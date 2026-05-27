#pragma once
#include <vector>
#include <shared_mutex>
#include <atomic>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include "mask.hpp"
#include "types.hpp"

namespace voxyl::ecs {

    class Archetype {
    public:
        Mask mask;
        std::vector<Entity> entities;
        std::vector<std::vector<std::uint8_t>> storage;
        std::vector<VkBuffer> buffers;
        std::vector<std::size_t> sizes;
        std::unordered_map<std::uint32_t, std::size_t> mapping;
        std::unordered_map<std::uint32_t, Archetype*> grow;
        std::unordered_map<std::uint32_t, Archetype*> shrink;
        std::atomic<std::uint32_t> version{0};
        mutable std::shared_mutex mutex;

        void add(Entity entity);
        std::uint32_t remove(std::size_t index);
    };

}