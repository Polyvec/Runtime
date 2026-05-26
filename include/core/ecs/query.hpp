#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <vulkan/vulkan.h>
#include "archetype.hpp"

namespace voxyl::ecs {

    class Query {
    public:
        using Callback = std::function<void(std::size_t count, const Entity* entities, const std::vector<void*>& components)>;

        explicit Query(const std::vector<std::unique_ptr<Archetype>>& list) noexcept;

        Query& with(std::uint32_t target);
        Query& without(std::uint32_t target);
        Query& any(const std::vector<std::uint32_t>& targets);

        bool has(std::uint32_t target) const noexcept;

        void run(const Callback& callback, const std::vector<std::uint32_t>& targets) const;

        void dispatch(VkCommandBuffer cmd, VkPipelineLayout layout, VkPipeline pipeline, const std::vector<std::uint32_t>& targets) const;

        const std::vector<std::uint32_t>& tracked() const noexcept { return order; }

    private:
        const std::vector<std::unique_ptr<Archetype>>& archetypes;
        Mask require;
        Mask reject;
        Mask allow;
        std::vector<std::uint32_t> order;
    };

}