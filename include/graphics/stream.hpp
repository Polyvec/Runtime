#pragma once

#include "context.hpp"
#include "buffer.hpp"
#include <memory>

namespace voxyl::graphics {

    class Stream {
    public:
        Stream(const Context& context, VkDeviceSize size, VkBufferUsageFlags usage);
        ~Stream() = default;

        void clear() noexcept;
        void upload(const void* data, VkDeviceSize size);

        [[nodiscard]] const Buffer& storage() const noexcept { return *buffer; }
        [[nodiscard]] VkDeviceSize offset() const noexcept { return position; }

    private:
        std::unique_ptr<Buffer> buffer;
        VkDeviceSize capacity;
        VkDeviceSize position;
    };

}