#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <vulkan/vulkan.h>
#include "kind.hpp"

namespace core {

    class Find {
    public:
        using Call = std::function<void(std::size_t size, const Id* list, const std::vector<void*>& tabs)>;

        explicit Find(const std::vector<std::unique_ptr<Kind>>& pool) noexcept;

        Find& with(std::uint32_t tag);
        Find& skip(std::uint32_t tag);
        Find& some(const std::vector<std::uint32_t>& tags);
        Find& sort(std::uint32_t tag);

        [[nodiscard]] bool test(std::uint32_t tag) const noexcept;

        void exec(const Call& call, const std::vector<std::uint32_t>& tags) const;

        void send(VkCommandBuffer stream, VkPipelineLayout layout, VkPipeline pipe, const std::vector<std::uint32_t>& tags) const;

        [[nodiscard]] const std::vector<std::uint32_t>& plan() const noexcept { return path; }

    private:
        const std::vector<std::unique_ptr<Kind>>& pool;
        Mask have;
        Mask lacks;
        Mask wish;
        std::vector<std::uint32_t> path;
    };

}