#include "../include/core/ecs/find.hpp"
#include <shared_mutex>
#include <algorithm>
#include <numeric>
#include <cstring>

namespace core {

    Find::Find(const std::vector<std::unique_ptr<Kind>>& pool) noexcept
        : pool(pool) {}

    Find& Find::with(std::uint32_t tag) {
        have.set(tag);
        path.push_back(tag);
        return *this;
    }

    Find& Find::skip(std::uint32_t tag) {
        lacks.set(tag);
        return *this;
    }

    Find& Find::some(const std::vector<std::uint32_t>& tags) {
        for (std::uint32_t tag : tags) {
            wish.set(tag);
        }
        return *this;
    }

    Find& Find::sort(std::uint32_t tag) {
        for (const auto& arch : pool) {
            std::unique_lock lock(arch->lock);
            Mask check = arch->mask;
            if ((check & have) == have && check.test(tag)) {
                std::size_t offset = arch->maps[tag];
                std::size_t size = arch->bits[offset];
                if (size == 0) continue;

                std::vector<std::size_t> indices(arch->rows.size());
                std::iota(indices.begin(), indices.end(), 0);

                std::uint8_t* raw = arch->page[offset].data();
                std::ranges::sort(indices, [raw, size](std::size_t left, std::size_t right) {
                    return std::memcmp(raw + left * size, raw + right * size, size) < 0;
                });

                std::vector<Id> arranged(arch->rows.size());
                for (std::size_t index = 0; index < indices.size(); ++index) {
                    arranged[index] = arch->rows[indices[index]];
                }
                arch->rows = std::move(arranged);

                for (std::size_t track = 0; track < arch->page.size(); ++track) {
                    std::size_t byte = arch->bits[track];
                    if (byte == 0) continue;
                    std::vector<std::uint8_t> linear(arch->page[track].size());
                    for (std::size_t index = 0; index < indices.size(); ++index) {
                        std::memcpy(linear.data() + index * byte,
                                    arch->page[track].data() + indices[index] * byte,
                                    byte);
                    }
                    arch->page[track] = std::move(linear);
                }
                arch->sync.fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    bool Find::test(std::uint32_t tag) const noexcept {
        return have.test(tag);
    }

    void Find::exec(const Call& call, const std::vector<std::uint32_t>& tags) const {
        std::vector<void*> pointers;
        for (const auto& arch : pool) {
            std::shared_lock lock(arch->lock);
            Mask check = arch->mask;
            if ((check & have) == have && (check & lacks).none()) {
                if (wish.any() && (check & wish).none()) continue;

                std::size_t total = arch->rows.size();
                if (total == 0) continue;

                pointers.clear();
                for (std::uint32_t tag : tags) {
                    auto find = arch->maps.find(tag);
                    if (find != arch->maps.end()) {
                        pointers.push_back(arch->page[find->second].data());
                    } else {
                        pointers.push_back(nullptr);
                    }
                }
                call(total, arch->rows.data(), pointers);
            }
        }
    }

    void Find::send(VkCommandBuffer stream, VkPipelineLayout layout, VkPipeline pipe, const std::vector<std::uint32_t>& tags) const {
        (void)layout;
        (void)tags;
        vkCmdBindPipeline(stream, VK_PIPELINE_BIND_POINT_COMPUTE, pipe);
        for (const auto& arch : pool) {
            Mask check = arch->mask;
            if ((check & have) == have && (check & lacks).none()) {
                std::shared_lock lock(arch->lock);
                auto count = static_cast<std::uint32_t>(arch->rows.size());
                if (count == 0) continue;

                vkCmdDispatch(stream, (count + 63) / 64, 1, 1);
            }
        }
    }

}