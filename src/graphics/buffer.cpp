#include "../include/graphics/buffer.hpp"
#include "../include/graphics/context.hpp"
#include <stdexcept>
#include <cstring>

namespace voxyl::graphics {

    Buffer::Buffer(const Context& context, const VkDeviceSize size, VkBufferUsageFlags usage, const VkMemoryPropertyFlags flags)
        : device(context.device()), hardware(context.hardware()), buffer(VK_NULL_HANDLE), memory(VK_NULL_HANDLE), bytes(size), address(nullptr) {

        VkBufferCreateInfo create{};
        create.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        create.size = size;
        create.usage = usage;
        create.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &create, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failure creating Vulkan buffer.");
        }

        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(device, buffer, &requirements);

        VkMemoryAllocateInfo allocate{};
        allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocate.allocationSize = requirements.size;
        allocate.memoryTypeIndex = find(requirements.memoryTypeBits, flags);

        if (vkAllocateMemory(device, &allocate, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("Failure allocating Vulkan buffer memory.");
        }

        vkBindBufferMemory(device, buffer, memory, 0);

        if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            vkMapMemory(device, memory, 0, size, 0, &address);
        }
    }

    Buffer::~Buffer() {
        if (device != VK_NULL_HANDLE) {
            if (buffer != VK_NULL_HANDLE) vkDestroyBuffer(device, buffer, nullptr);
            if (memory != VK_NULL_HANDLE) vkFreeMemory(device, memory, nullptr);
        }
    }

    Buffer::Buffer(Buffer&& other) noexcept
        : device(other.device), hardware(other.hardware), buffer(other.buffer), memory(other.memory), bytes(other.bytes), address(other.address) {
        other.device = VK_NULL_HANDLE;
        other.hardware = VK_NULL_HANDLE;
        other.buffer = VK_NULL_HANDLE;
        other.memory = VK_NULL_HANDLE;
        other.address = nullptr;
        other.bytes = 0;
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept {
        if (this != &other) {
            if (device != VK_NULL_HANDLE) {
                if (buffer != VK_NULL_HANDLE) vkDestroyBuffer(device, buffer, nullptr);
                if (memory != VK_NULL_HANDLE) vkFreeMemory(device, memory, nullptr);
            }
            device = other.device;
            hardware = other.hardware;
            buffer = other.buffer;
            memory = other.memory;
            bytes = other.bytes;
            address = other.address;

            other.device = VK_NULL_HANDLE;
            other.hardware = VK_NULL_HANDLE;
            other.buffer = VK_NULL_HANDLE;
            other.memory = VK_NULL_HANDLE;
            other.address = nullptr;
            other.bytes = 0;
        }
        return *this;
    }

    void Buffer::map(const VkDeviceSize offset, const VkDeviceSize size) {
        if (!address) {
            vkMapMemory(device, memory, offset, size, 0, &address);
        }
    }

    void Buffer::unmap() {
        if (address) {
            vkUnmapMemory(device, memory);
            address = nullptr;
        }
    }

    void Buffer::raw(const void* data, const VkDeviceSize size, const VkDeviceSize offset) const {
        if (address) {
            std::memcpy(static_cast<char*>(address) + offset, data, size);
        } else {
            void* temporary = nullptr;
            vkMapMemory(device, memory, offset, size, 0, &temporary);
            std::memcpy(temporary, data, size);
            vkUnmapMemory(device, memory);
        }
    }

    uint32_t Buffer::find(const uint32_t filter, const VkMemoryPropertyFlags flags) const {
        VkPhysicalDeviceMemoryProperties properties;
        vkGetPhysicalDeviceMemoryProperties(hardware, &properties);
        for (uint32_t index = 0; index < properties.memoryTypeCount; index++) {
            if ((filter & (1 << index)) && (properties.memoryTypes[index].propertyFlags & flags) == flags) {
                return index;
            }
        }
        throw std::runtime_error("Failure finding suitable memory type.");
    }

}