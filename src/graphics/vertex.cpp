#include "../include/graphics/vertex.hpp"

namespace voxyl::graphics {

    VkVertexInputBindingDescription Vertex::binding() {
        VkVertexInputBindingDescription info{};
        info.binding = 0;
        info.stride = sizeof(Vertex);
        info.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return info;
    }

    std::vector<VkVertexInputAttributeDescription> Vertex::attributes() {
        return {
                    {
                        .location = 0,
                        .binding = 0,
                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset = 0,
                    },
                    {
                        .location = 1,
                        .binding = 0,
                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset = static_cast<uint32_t>(sizeof(math::Vector3)),
                    },
                    {
                        .location = 2,
                        .binding = 0,
                        .format = VK_FORMAT_R32G32_SFLOAT,
                        .offset = static_cast<uint32_t>(sizeof(math::Vector3) + sizeof(math::Vector3)),
                    }
        };
    }

}