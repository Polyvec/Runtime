#include "../../include/ecs/query.hpp"
#include <shared_mutex>
#include <future>

namespace voxyl::ecs {

    Query::Query(const std::vector<std::unique_ptr<Archetype>>& list) noexcept
        : archetypes(list) {}

    Query& Query::with(const std::uint32_t target) {
        require.set(target);
        order.push_back(target);
        return *this;
    }

    Query& Query::without(const std::uint32_t target) {
        reject.set(target);
        return *this;
    }

    Query& Query::any(const std::vector<std::uint32_t>& targets) {
        for (const std::uint32_t target : targets) {
            allow.set(target);
        }
        return *this;
    }

    bool Query::has(const std::uint32_t target) const noexcept {
        return require.test(target);
    }

    void Query::run(const Callback& callback, const std::vector<std::uint32_t>& targets) const {
        std::vector<std::future<void>> tasks;

        for (const auto& archetype : archetypes) {
            if ((archetype->mask & require) != require) continue;
            if ((archetype->mask & reject).any()) continue;
            if (allow.any() && (archetype->mask & allow).none()) continue;

            tasks.push_back(std::async(std::launch::async, [pointer = archetype.get(), callback, targets]() {
                std::shared_lock lock(pointer->mutex);
                if (pointer->entities.empty()) return;

                std::vector<void*> pointers;
                pointers.reserve(targets.size());

                for (const std::uint32_t target : targets) {
                    auto item = pointer->mapping.find(target);
                    if (item == pointer->mapping.end()) break;
                    pointers.push_back(pointer->storage[item->second].data());
                }

                if (pointers.size() == targets.size()) {
                    callback(pointer->entities.size(), pointer->entities.data(), pointers);
                }
            }));
        }

        for (auto& task : tasks) {
            if (task.valid()) {
                task.get();
            }
        }
    }

    void Query::dispatch(VkCommandBuffer cmd, VkPipelineLayout layout, VkPipeline pipeline, const std::vector<std::uint32_t>& targets) const {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

        for (const auto& archetype : archetypes) {
            if ((archetype->mask & require) != require) continue;
            if ((archetype->mask & reject).any()) continue;
            if (allow.any() && (archetype->mask & allow).none()) continue;

            std::shared_lock lock(archetype->mutex);
            std::uint32_t count = static_cast<std::uint32_t>(archetype->entities.size());
            if (count == 0) continue;

            std::vector<VkDescriptorBufferInfo> infos;
            infos.reserve(targets.size());

            for (const std::uint32_t target : targets) {
                auto item = archetype->mapping.find(target);
                if (item == archetype->mapping.end()) break;

                VkBuffer buffer = archetype->buffers[item->second];
                infos.push_back(VkDescriptorBufferInfo{buffer, 0, VK_WHOLE_SIZE});
            }

            if (infos.size() != targets.size()) continue;

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = nullptr;
            write.dstBinding = 0;
            write.descriptorCount = static_cast<std::uint32_t>(infos.size());
            write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            write.pBufferInfo = infos.data();

            vkCmdPushConstants(cmd, layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(std::uint32_t), &count);

            std::uint32_t groups = (count + 63) / 64;
            vkCmdDispatch(cmd, groups, 1, 1);
        }
    }

}