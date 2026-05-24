#include "../include/graphics/stream.hpp"
#include <stdexcept>
#include <cstring>

namespace voxyl::graphics {

    Stream::Stream(const Context& context, VkDeviceSize size, VkBufferUsageFlags usage)
        : buffer(std::make_unique<Buffer>(context, size, usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)), capacity(size), position(0) {
    }

    void Stream::clear() noexcept {
        position = 0;
    }

    void Stream::upload(const void* data, const VkDeviceSize size) {
        if (position + size > capacity) {
            throw std::runtime_error("Overflow encountered inside fast memory stream buffer storage reallocation context.");
        }

        void* target = static_cast<char*>(buffer->pointer()) + position;
        std::memcpy(target, data, size);
        position += size;
    }

}