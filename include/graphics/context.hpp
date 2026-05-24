#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "../include/math/vector2.hpp"
#include "../include/math/vector3.hpp"
#include "../include/math/matrix4.hpp"
#include "../include/math/quaternion.hpp"
#include "../include/math/transform.hpp"

namespace voxyl::graphics {

    class Pipeline;
    class Buffer;
    class Texture;

    struct Camera {
        bool flat;
        math::Vector3 position;
        math::Quaternion rotation;
        float fov;
        float aspect;
    };

    struct Model {
        const Texture* texture;
        math::Transform transform;
    };

    struct Sprite {
        const Texture* texture;
        math::Vector2 position;
        math::Vector2 size;
        float rotation;
    };

    class Context {
    public:
        Context(int width, int height);
        ~Context();

        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;

        void begin();
        void end();

        void view(const Camera& camera);
        void draw(const Model& model);
        void draw(const Sprite& sprite);

        [[nodiscard]] VkDevice device() const { return core; }
        [[nodiscard]] VkPhysicalDevice hardware() const { return silicon; }

    private:
        void init(int width, int height);
        void link();
        void clean() const;

        VkInstance instance;
        VkPhysicalDevice silicon;
        VkDevice core;
        VkQueue graphics;

        std::vector<VkImage> images;
        std::vector<VkDeviceMemory> memories;
        std::vector<VkImageView> views;
        VkFormat format;
        VkExtent2D extent;

        VkCommandPool pool;
        std::vector<VkCommandBuffer> buffers;
        std::vector<VkFence> fences;

        int tick;
        uint32_t index;
        bool resized;

        math::Matrix4 matrix;
    };

}