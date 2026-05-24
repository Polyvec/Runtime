#include "texture.hpp"
#include "../include/graphics/context.hpp"
#include <stdexcept>

namespace voxyl::graphics {

    Texture::Texture(const Context& context, const std::string& path)
        : core(context.device()), silicon(context.hardware()), pool(VK_NULL_HANDLE), queue(VK_NULL_HANDLE),
          width(0), height(0), mips(1), image(VK_NULL_HANDLE), memory(VK_NULL_HANDLE), lens(VK_NULL_HANDLE), probe(VK_NULL_HANDLE) {
        generate(256, 256, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        expose(VK_FORMAT_R8G8B8A8_SRGB);
        sample();
    }

    Texture::~Texture() {
        vkDestroySampler(core, probe, nullptr);
        vkDestroyImageView(core, lens, nullptr);
        vkDestroyImage(core, image, nullptr);
        vkFreeMemory(core, memory, nullptr);
    }

    Texture::Texture(Texture&& other) noexcept
        : core(other.core), silicon(other.silicon), pool(other.pool), queue(other.queue),
          width(other.width), height(other.height), mips(other.mips), image(other.image),
          memory(other.memory), lens(other.lens), probe(other.probe) {
        other.image = VK_NULL_HANDLE;
        other.memory = VK_NULL_HANDLE;
        other.lens = VK_NULL_HANDLE;
        other.probe = VK_NULL_HANDLE;
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        if (this != &other) {
            vkDestroySampler(core, probe, nullptr);
            vkDestroyImageView(core, lens, nullptr);
            vkDestroyImage(core, image, nullptr);
            vkFreeMemory(core, memory, nullptr);

            core = other.core;
            silicon = other.silicon;
            pool = other.pool;
            queue = other.queue;
            width = other.width;
            height = other.height;
            mips = other.mips;
            image = other.image;
            memory = other.memory;
            lens = other.lens;
            probe = other.probe;

            other.image = VK_NULL_HANDLE;
            other.memory = VK_NULL_HANDLE;
            other.lens = VK_NULL_HANDLE;
            other.probe = VK_NULL_HANDLE;
        }
        return *this;
    }

    void Texture::generate(uint32_t x, uint32_t y, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags flags) {
        width = x;
        height = y;

        VkImageCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.extent.width = width;
        info.extent.height = height;
        info.extent.depth = 1;
        info.mipLevels = mips;
        info.arrayLayers = 1;
        info.format = format;
        info.tiling = tiling;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.usage = usage;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(core, &info, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("Texture resource creation failed");
        }

        VkMemoryRequirements requirements;
        vkGetImageMemoryRequirements(core, image, &requirements);

        VkPhysicalDeviceMemoryProperties properties;
        vkGetPhysicalDeviceMemoryProperties(silicon, &properties);

        uint32_t type = 0;
        bool matching = false;
        for (uint32_t index = 0; index < properties.memoryTypeCount; index++) {
            if ((requirements.memoryTypeBits & (1 << index)) && (properties.memoryTypes[index].propertyFlags & flags) == flags) {
                type = index;
                matching = true;
                break;
            }
        }

        if (!matching) {
            throw std::runtime_error("Texture memory layout selection failed");
        }

        VkMemoryAllocateInfo allocation{};
        allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocation.allocationSize = requirements.size;
        allocation.memoryTypeIndex = type;

        if (vkAllocateMemory(core, &allocation, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("Texture memory allocation failed");
        }

        vkBindImageMemory(core, image, memory, 0);
    }

    void Texture::expose(VkFormat format) {
        VkImageViewCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image = image;
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = format;
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = mips;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(core, &info, nullptr, &lens) != VK_SUCCESS) {
            throw std::runtime_error("Texture target view allocation failed");
        }
    }

    void Texture::sample() {
        VkSamplerCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.magFilter = VK_FILTER_LINEAR;
        info.minFilter = VK_FILTER_LINEAR;
        info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.anisotropyEnable = VK_FALSE;
        info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        info.unnormalizedCoordinates = VK_FALSE;
        info.compareEnable = VK_FALSE;
        info.compareOp = VK_COMPARE_OP_ALWAYS;
        info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        if (vkCreateSampler(core, &info, nullptr, &probe) != VK_SUCCESS) {
            throw std::runtime_error("Texture graphics sampler assignment failed");
        }
    }

    void Texture::transition(VkFormat format, VkImageLayout past, VkImageLayout future) {
    }

    void Texture::copy(VkBuffer source, uint32_t x, uint32_t y) {
    }

}