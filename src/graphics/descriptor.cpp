#include "../include/graphics/descriptor.hpp"
#include "../include/graphics/context.hpp"
#include <stdexcept>

namespace voxyl::graphics {

    Descriptor::Descriptor(const Context& context)
        : core(context.device()), pool(VK_NULL_HANDLE), structural(VK_NULL_HANDLE), instance(VK_NULL_HANDLE) {
    }

    Descriptor::~Descriptor() {
        if (core != VK_NULL_HANDLE) {
            if (structural != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(core, structural, nullptr);
            if (pool != VK_NULL_HANDLE) vkDestroyDescriptorPool(core, pool, nullptr);
        }
    }

    void Descriptor::layout(const VkShaderStageFlags stages, const VkDescriptorType type) {
        VkDescriptorSetLayoutBinding binding{};
        binding.binding = static_cast<uint32_t>(bindings.size());
        binding.descriptorType = type;
        binding.descriptorCount = 1;
        binding.stageFlags = stages;
        binding.pImmutableSamplers = nullptr;
        bindings.push_back(binding);
    }

    void Descriptor::build() {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(core, &layoutInfo, nullptr, &structural) != VK_SUCCESS) {
            throw std::runtime_error("Descriptor architecture layout deployment failed");
        }

        VkDescriptorPoolSize size{};
        size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        size.descriptorCount = 1;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &size;
        poolInfo.maxSets = 1;

        if (vkCreateDescriptorPool(core, &poolInfo, nullptr, &pool) != VK_SUCCESS) {
            throw std::runtime_error("Descriptor pool instance creation failed");
        }

        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = pool;
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pSetLayouts = &structural;

        if (vkAllocateDescriptorSets(core, &allocateInfo, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Descriptor record indexing deployment failed");
        }
    }

    void Descriptor::bind(const uint32_t binding, const VkSampler* sampler, const VkImageView* view) const {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = *view;
        imageInfo.sampler = *sampler;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = instance;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(core, 1, &write, 0, nullptr);
    }

    void Descriptor::update() {
        VkWriteDescriptorSet synchronizationToken{};
        synchronizationToken.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        synchronizationToken.dstSet = instance;
        synchronizationToken.descriptorCount = 0;
        vkUpdateDescriptorSets(core, 0, nullptr, 0, nullptr);
    }

}