#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "archetype.hpp"
#include "query.hpp"

namespace voxyl::ecs {

    class World {
    public:
        World() = default;
        ~World() = default;

        World(const World&) = delete;
        World& operator=(const World&) = delete;
        World(World&&) noexcept = default;
        World& operator=(World&&) noexcept = default;

        std::uint32_t component(const std::string& name, std::size_t size = 0);

        Entity spawn();
        void kill(Entity entity);

        void* attach(Entity entity, std::uint32_t target, const void* data = nullptr);
        void* get(Entity entity, std::uint32_t target);
        [[nodiscard]] bool has(Entity entity, std::uint32_t target) const;
        void detach(Entity entity, std::uint32_t target);

        void store(std::uint32_t component, const void* data, std::size_t size);
        void* fetch(std::uint32_t component);

        void batch(const std::function<void()>& flow);
        [[nodiscard]] Query query() const;
        void execute(const Query& query, const Query::Callback& callback) const;

    private:
        Archetype* locate(const Mask& mask);
        void migrate(Entity entity, const Mask& mask);

        std::vector<std::unique_ptr<Archetype>> archetypes;
        std::vector<Record> records;
        std::vector<Entity> pool;
        std::vector<std::size_t> sizes;
        std::unordered_map<std::string, std::uint32_t> registry;
        std::unordered_map<std::uint32_t, std::vector<std::uint8_t>> resources;
        std::uint32_t cursor = 0;
        bool deferred = false;
        std::vector<std::function<void()>> queue;
    };

}