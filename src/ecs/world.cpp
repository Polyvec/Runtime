#include "../../include/ecs/world.hpp"
#include <cstring>

namespace voxyl::ecs {

    std::uint32_t World::component(const std::string& name, const std::size_t size) {
        auto item = registry.find(name);
        if (item != registry.end()) {
            return item->second;
        }
        sizes.push_back(size);
        const std::uint32_t id = static_cast<std::uint32_t>(sizes.size() - 1);
        registry[name] = id;
        return id;
    }

    Entity World::spawn() {
        Entity entity;
        if (!pool.empty()) {
            entity = pool.back();
            pool.pop_back();
        } else {
            entity = cursor++;
        }

        if (entity >= records.size()) {
            records.resize(entity + 1);
        }

        constexpr Mask mask;
        migrate(entity, mask);
        return entity;
    }

    void World::kill(const Entity entity) {
        if (deferred) {
            queue.push_back([this, entity]() {
                this->kill(entity);
            });
            return;
        }

        auto [archetype, index] = records[entity];
        if (archetype) {
            const Entity swapped = archetype->remove(index);
            if (swapped != NONE) {
                records[swapped].index = index;
            }
        }
        records[entity] = {nullptr, 0};
        pool.push_back(entity);
    }

    void* World::attach(const Entity entity, const std::uint32_t target, const void* data) {
        if (deferred) {
            std::vector<std::uint8_t> bytes;
            if (data) {
                bytes.resize(sizes[target]);
                std::memcpy(bytes.data(), data, sizes[target]);
            }
            queue.push_back([this, entity, target, bytes = std::move(bytes)]() {
                this->attach(entity, target, bytes.empty() ? nullptr : bytes.data());
            });
            return nullptr;
        }

        Mask mask = records[entity].archetype ? records[entity].archetype->mask : Mask();
        mask.set(target);
        migrate(entity, mask);

        auto [archetype, index] = records[entity];
        const std::size_t column = archetype->mapping[target];
        void* memory = &archetype->storage[column][index * sizes[target]];

        if (data) {
            std::memcpy(memory, data, sizes[target]);
        }

        return memory;
    }

    void* World::get(const Entity entity, const std::uint32_t target) {
        auto [archetype, index] = records[entity];
        if (!archetype || !archetype->mask.test(target)) {
            return nullptr;
        }
        const std::size_t column = archetype->mapping[target];
        return &archetype->storage[column][index * sizes[target]];
    }

    bool World::has(const Entity entity, const std::uint32_t target) const {
        if (entity >= records.size()) return false;
        auto [archetype, index] = records[entity];
        return archetype && archetype->mask.test(target);
    }

    void World::detach(const Entity entity, const std::uint32_t target) {
        if (deferred) {
            queue.push_back([this, entity, target]() {
                this->detach(entity, target);
            });
            return;
        }

        auto [archetype, index] = records[entity];
        if (!archetype || !archetype->mask.test(target)) return;

        Mask mask = archetype->mask;
        mask.reset(target);
        migrate(entity, mask);
    }

    void World::batch(const std::function<void()>& flow) {
        deferred = true;
        flow();
        deferred = false;

        for (const auto& operation : queue) {
            operation();
        }
        queue.clear();
    }

    Query World::query() const {
        return Query(archetypes);
    }

    void World::execute(const Query& query, const Query::Callback& callback) const {
        query.run(callback, query.tracked());
    }

    Archetype* World::locate(const Mask& mask) {
        for (const auto& archetype : archetypes) {
            if (archetype->mask == mask) {
                return archetype.get();
            }
        }

        auto archetype = std::make_unique<Archetype>();
        archetype->mask = mask;

        for (std::size_t index = 0; index < sizes.size(); ++index) {
            if (mask.test(index)) {
                archetype->mapping[index] = archetype->storage.size();
                archetype->storage.emplace_back();
                archetype->sizes.push_back(sizes[index]);
                archetype->buffers.emplace_back(static_cast<VkBuffer>(VK_NULL_HANDLE));
            }
        }
        archetypes.push_back(std::move(archetype));
        return archetypes.back().get();
    }

    void World::migrate(const Entity entity, const Mask& mask) {
        auto [archetype, index] = records[entity];
        Archetype* destination = locate(mask);

        if (archetype == destination) {
            return;
        }

        destination->add(entity);
        const std::size_t slot = destination->entities.size() - 1;

        if (archetype) {
            {
                std::shared_lock lock(archetype->mutex);
                for (auto const& [target, column] : archetype->mapping) {
                    if (destination->mapping.contains(target)) {
                        const std::size_t lane = destination->mapping[target];
                        std::memcpy(
                            &destination->storage[lane][slot * sizes[target]],
                            &archetype->storage[column][index * sizes[target]],
                            sizes[target]
                        );
                    }
                }
            }

            const Entity swapped = archetype->remove(index);
            if (swapped != NONE) {
                records[swapped].index = index;
            }
        }

        records[entity] = {destination, slot};
    }

}