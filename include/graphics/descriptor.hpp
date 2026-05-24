#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace voxyl::graphics {

    class Context;

    class Descriptor {
    public:
        explicit Descriptor(const Context& context);
        ~Descriptor();

        Descriptor(const Descriptor&) = delete;
        Descriptor& operator=(const Descriptor&) = delete;

        void layout(VkShaderStageFlags stages, VkDescriptorType type);
        void build();
        void bind(uint32_t binding, const VkSampler* sampler, const VkImageView* view) const;
        void update();

        [[nodiscard]] VkDescriptorSetLayout architecture() const { return structural; }
        [[nodiscard]] VkDescriptorSet set() const { return instance; }

    private:
        VkDevice core;
        VkDescriptorPool pool;
        VkDescriptorSetLayout structural;
        VkDescriptorSet instance;
        std::vector<VkDescriptorSetLayoutBinding> bindings;
    };

}