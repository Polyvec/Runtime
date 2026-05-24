#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "../include/math/vector2.hpp"
#include "../include/math/vector3.hpp"

namespace voxyl::graphics {

    struct Vertex {
        math::Vector3 position;
        math::Vector3 color;
        math::Vector2 uv;

        static VkVertexInputBindingDescription binding();
        static std::vector<VkVertexInputAttributeDescription> attributes();
    };

}