#pragma once

#include <vulkan/vulkan.h>
#include <span>

namespace voxyl::graphics {

    class Context;

    class Buffer {
    public:
        Buffer(const Context& context, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags);
        ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&& other) noexcept;
        Buffer& operator=(Buffer&& other) noexcept;

        void map(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
        void unmap();

        template<typename T>
        void write(std::span<const T> data, const VkDeviceSize offset = 0) {
            raw(data.data(), data.size_bytes(), offset);
        }

        [[nodiscard]] void* pointer() const noexcept { return address; }
        [[nodiscard]] VkBuffer handle() const noexcept { return buffer; }
        [[nodiscard]] VkDeviceSize size() const noexcept { return bytes; }

    private:
        void raw(const void* data, VkDeviceSize size, VkDeviceSize offset) const;
        [[nodiscard]] uint32_t find(uint32_t filter, VkMemoryPropertyFlags flags) const;

        VkDevice device;
        VkPhysicalDevice hardware;
        VkBuffer buffer;
        VkDeviceMemory memory;
        VkDeviceSize bytes;
        void* address;
    };

}