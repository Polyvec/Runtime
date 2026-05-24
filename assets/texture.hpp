#pragma once

#include <vulkan/vulkan.h>
#include <string>

namespace voxyl::graphics {

    class Context;
    class Buffer;

    class Texture {
    public:
        Texture(const Context& context, const std::string& path);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        [[nodiscard]] VkImageView view() const { return lens; }
        [[nodiscard]] VkSampler sampler() const { return probe; }
        [[nodiscard]] uint32_t span() const { return width; }
        [[nodiscard]] uint32_t drop() const { return height; }

    private:
        void generate(uint32_t x, uint32_t y, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags flags);
        void expose(VkFormat format);
        void sample();
        void transition(VkFormat format, VkImageLayout past, VkImageLayout future);
        void copy(VkBuffer source, uint32_t x, uint32_t y);

        VkDevice core;
        VkPhysicalDevice silicon;
        VkCommandPool pool;
        VkQueue queue;

        uint32_t width;
        uint32_t height;
        uint32_t mips;

        VkImage image;
        VkDeviceMemory memory;
        VkImageView lens;
        VkSampler probe;
    };

}