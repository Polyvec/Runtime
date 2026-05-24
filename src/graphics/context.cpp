#include "../include/graphics/context.hpp"
#include "../include/math/matrix4.hpp"
#include <stdexcept>

namespace voxyl::graphics {

    Context::Context(const int width, const int height)
        : instance(VK_NULL_HANDLE), silicon(VK_NULL_HANDLE), core(VK_NULL_HANDLE),
          graphics(VK_NULL_HANDLE), format(VK_FORMAT_B8G8R8A8_SRGB),
          extent{static_cast<uint32_t>(width), static_cast<uint32_t>(height)},
          pool(VK_NULL_HANDLE), tick(0), index(0), resized(false),
          matrix(math::Matrix4::IDENTITY) {
        init(width, height);
        link();
    }

    Context::~Context() {
        clean();
    }

    void Context::init(int width, int height) {
        (void)width; (void)height;

        VkApplicationInfo app{};
        app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app.pApplicationName = "Voxyl Engine";
        app.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app.pEngineName = "Voxyl Graphics Engine";
        app.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo create{};
        create.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create.pApplicationInfo = &app;

        if (vkCreateInstance(&create, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan instance.");
        }

        uint32_t count = 0;
        vkEnumeratePhysicalDevices(instance, &count, nullptr);
        if (count == 0) {
            throw std::runtime_error("No available physical hardware GPUs support Vulkan.");
        }

        std::vector<VkPhysicalDevice> devices(count);
        vkEnumeratePhysicalDevices(instance, &count, devices.data());
        silicon = devices[0];

        float priority = 1.0f;
        VkDeviceQueueCreateInfo queue{};
        queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue.queueFamilyIndex = 0;
        queue.queueCount = 1;
        queue.pQueuePriorities = &priority;

        VkDeviceCreateInfo deviceCreate{};
        deviceCreate.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreate.queueCreateInfoCount = 1;
        deviceCreate.pQueueCreateInfos = &queue;

        if (vkCreateDevice(silicon, &deviceCreate, nullptr, &core) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device.");
        }

        vkGetDeviceQueue(core, 0, 0, &graphics);
    }

    void Context::link() {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = 0;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(core, &poolInfo, nullptr, &pool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool.");
        }

        buffers.resize(2);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = pool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(buffers.size());

        if (vkAllocateCommandBuffers(core, &allocInfo, buffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers.");
        }
    }

    void Context::clean() const {
        if (core != VK_NULL_HANDLE) {
            if (pool != VK_NULL_HANDLE) {
                vkDestroyCommandPool(core, pool, nullptr);
            }
            vkDestroyDevice(core, nullptr);
        }
        if (instance != VK_NULL_HANDLE) {
            vkDestroyInstance(instance, nullptr);
        }
    }

    void Context::begin() {
        vkResetCommandBuffer(buffers[index], 0);
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(buffers[index], &beginInfo);
    }

    void Context::end() {
        vkEndCommandBuffer(buffers[index]);

        VkSubmitInfo submit{};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &buffers[index];

        if (vkQueueSubmit(graphics, 1, &submit, VK_NULL_HANDLE) != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit command buffer.");
        }
        vkQueueWaitIdle(graphics);

        index = (index + 1) % buffers.size();
    }

    void Context::view(const Camera& camera) {
        math::Matrix4 view;
        math::Matrix4 projection;

        if (camera.flat) {
            view = math::Matrix4::translate(-camera.position);
            projection = math::Matrix4::orthographic(0.0f, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, -1.0f, 1.0f);
        } else {
            view = math::Matrix4::rotate(camera.rotation).inverted() * math::Matrix4::translate(-camera.position);
            projection = math::Matrix4::perspective(camera.fov, camera.aspect, 0.1f, 1000.0f);
        }

        matrix = projection * view;
    }

    void Context::draw(const Model& model) {
        math::Matrix4 world = model.transform.matrix();
        math::Matrix4 transform = matrix * world;

        vkCmdPushConstants(buffers[index], VK_NULL_HANDLE, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(math::Matrix4), &transform);
    }

    void Context::draw(const Sprite& sprite) {
        math::Matrix4 world = math::Matrix4::translate(math::Vector3(sprite.position, 0.0f)) * math::Matrix4::rotate(math::Quaternion::around(sprite.rotation, math::Vector3(0.0f, 0.0f, 1.0f))) * math::Matrix4::scale(math::Vector3(sprite.size, 1.0f));
        math::Matrix4 transform = matrix * world;

        vkCmdPushConstants(buffers[index], VK_NULL_HANDLE, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(math::Matrix4), &transform);
    }

}