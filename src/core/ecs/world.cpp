#include "../include/core/ecs/world.hpp"
#include <cstring>

namespace voxyl::ecs {

    std::uint32_t World::component(const std::string& name, const std::size_t size) {
        if (auto item = registry.find(name); item != registry.end()) {
            return item->second;
        }
        sizes.push_back(size);
        const auto id = static_cast<std::uint32_t>(sizes.size() - 1);
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
        Mask mask;
        migrate(entity, mask);
        return entity;
    }

    void World::kill(const Entity entity) {
        if (deferred) {
            queue.emplace_back([this, entity]() {
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

    void* World::attach(Entity entity, std::uint32_t target, const void* data) {
        if (deferred) {
            queue.emplace_back([this, entity, target, data]() {
                this->attach(entity, target, data);
            });
            return nullptr;
        }
        auto [archetype, index] = records[entity];
        Archetype* destination = nullptr;
        if (archetype) {
            auto item = archetype->grow.find(target);
            if (item != archetype->grow.end()) {
                destination = item->second;
            }
        }
        if (!destination) {
            Mask mask = archetype ? archetype->mask : Mask();
            mask.set(target);
            destination = locate(mask);
            if (archetype) {
                archetype->grow[target] = destination;
                destination->shrink[target] = archetype;
            }
        }
        migrate(entity, destination->mask);
        if (sizes[target] > 0) {
            void* pointer = get(entity, target);
            if (data && pointer) {
                std::memcpy(pointer, data, sizes[target]);
            }
            return pointer;
        }
        return nullptr;
    }

    void* World::get(Entity entity, std::uint32_t target) {
        auto [archetype, index] = records[entity];
        if (!archetype) {
            return nullptr;
        }
        auto item = archetype->mapping.find(target);
        if (item == archetype->mapping.end()) {
            return nullptr;
        }
        return &archetype->storage[item->second][index * sizes[target]];
    }

    bool World::has(Entity entity, std::uint32_t target) const {
        auto [archetype, index] = records[entity];
        if (!archetype) {
            return false;
        }
        return archetype->mask.test(target);
    }

    void World::detach(Entity entity, std::uint32_t target) {
        if (deferred) {
            queue.emplace_back([this, entity, target]() {
                this->detach(entity, target);
            });
            return;
        }
        auto [archetype, index] = records[entity];
        if (!archetype) {
            return;
        }
        Archetype* destination = nullptr;
        auto item = archetype->shrink.find(target);
        if (item != archetype->shrink.end()) {
            destination = item->second;
        }
        if (!destination) {
            Mask mask = archetype->mask;
            mask.reset(target);
            destination = locate(mask);
            archetype->shrink[target] = destination;
            destination->grow[target] = archetype;
        }
        migrate(entity, destination->mask);
    }

    void World::store(std::uint32_t component, const void* data, std::size_t size) {
        auto& block = resources[component];
        block.resize(size);
        std::memcpy(block.data(), data, size);
    }

    void* World::fetch(std::uint32_t component) {
        auto item = resources.find(component);
        if (item == resources.end()) {
            return nullptr;
        }
        return item->second.data();
    }

    void World::batch(const std::function<void()>& flow) {
        deferred = true;
        flow();
        deferred = false;
        for (const auto& action : queue) {
            action();
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
        for (auto& archetype : archetypes) {
            if (archetype->mask == mask) {
                return archetype.get();
            }
        }
        auto archetype = std::make_unique<Archetype>();
        archetype->mask = mask;
        for (std::uint32_t index = 0; index < MAX_COMPONENTS; ++index) {
            if (mask.test(index)) {
                if (sizes[index] > 0) {
                    archetype->mapping[index] = archetype->storage.size();
                    archetype->storage.emplace_back();
                    archetype->sizes.push_back(sizes[index]);
                    archetype->buffers.emplace_back(static_cast<VkBuffer>(VK_NULL_HANDLE));
                }
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