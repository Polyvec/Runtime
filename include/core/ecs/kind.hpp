#pragma once
#include <vector>
#include <shared_mutex>
#include <atomic>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include "mask.hpp"
#include "type.hpp"

namespace core {

    class Kind {
    public:
        Mask mask;
        std::vector<Id> rows;
        std::vector<std::vector<std::uint8_t>> page;
        std::vector<VkBuffer> vram;
        std::vector<std::size_t> bits;
        std::unordered_map<std::uint32_t, std::size_t> maps;
        std::unordered_map<std::uint32_t, Kind*> more;
        std::unordered_map<std::uint32_t, Kind*> less;
        std::atomic<std::uint32_t> sync{0};
        mutable std::shared_mutex lock;

        void push(Id id);
        std::uint32_t pull(std::size_t row);
    };

}