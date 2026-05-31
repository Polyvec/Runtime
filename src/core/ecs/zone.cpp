#include "../include/core/ecs/zone.hpp"
#include <cstring>

namespace core {

    std::uint32_t Zone::type(const std::string& name, std::size_t size) {
        auto find = keys.find(name);
        if (find != keys.end()) {
            return find->second;
        }
        auto id = static_cast<std::uint32_t>(bits.size());
        bits.push_back(size);
        keys[name] = id;

        if (name == "parent") {
            stem = id;
        } else if (name == "child") {
            twig = id;
        }
        return id;
    }

    Id Zone::make() {
        Id id;
        if (!dead.empty()) {
            id = dead.back();
            dead.pop_back();
        } else {
            id = next++;
        }
        if (id >= tabs.size()) {
            tabs.resize(id + 1);
        }
        Mask blank;
        Kind* arch = view(blank);
        arch->push(id);
        tabs[id] = {arch, arch->rows.size() - 1};
        return id;
    }

    Id Zone::twin(Id id) {
        Id duplicate = make();
        Slot source = tabs[id];
        Kind* arch = source.type;
        std::shared_lock lock(arch->lock);

        Mask mask = arch->mask;
        move(duplicate, mask);

        Slot destination = tabs[duplicate];
        Kind* goal = destination.type;
        std::unique_lock write(goal->lock);

        for (auto const& [tag, track] : arch->maps) {
            std::size_t size = bits[tag];
            if (size > 0) {
                std::size_t offset = goal->maps[tag];
                std::memcpy(goal->page[offset].data() + destination.row * size,
                            arch->page[track].data() + source.row * size,
                            size);
            }
        }
        return duplicate;
    }

    void Zone::kill(Id id) {
        if (hold) {
            jobs.emplace_back([this, id]() { kill(id); });
            return;
        }
        Slot record = tabs[id];
        if (!record.type) return;

        if (twig != 0xFFFFFFFF && test(id, twig)) {
            auto* list = static_cast<std::vector<Id>*>(peek(id, twig));
            if (list) {
                for (Id nested : *list) {
                    kill(nested);
                }
            }
        }

        std::uint32_t moved = record.type->pull(record.row);
        if (moved != id) {
            tabs[moved].row = record.row;
        }
        tabs[id] = {nullptr, 0};
        dead.push_back(id);
    }

    void Zone::wipe() {
        pool.clear();
        tabs.clear();
        dead.clear();
        vats.clear();
        acts.clear();
        jobs.clear();
        next = 0;
        hold = false;
    }

    void Zone::weld(Id item, Id boss) {
        if (stem == 0xFFFFFFFF) type("parent", sizeof(Id));
        if (twig == 0xFFFFFFFF) type("child", sizeof(std::vector<Id>));

        fill(item, stem, &boss);

        if (!test(boss, twig)) {
            std::vector<Id> list;
            list.push_back(item);
            fill(boss, twig, &list);
        } else {
            auto* list = static_cast<std::vector<Id>*>(peek(boss, twig));
            list->push_back(item);
        }
    }

    Id Zone::boss(Id id) const {
        if (stem == 0xFFFFFFFF || !test(id, stem)) return Null;
        return *static_cast<const Id*>(const_cast<Zone*>(this)->peek(id, stem));
    }

    const std::vector<Id>& Zone::crew(Id id) const {
        static const std::vector<Id> empty;
        if (twig == 0xFFFFFFFF || !test(id, twig)) return empty;
        return *static_cast<const std::vector<Id>*>(const_cast<Zone*>(this)->peek(id, twig));
    }

    void* Zone::fill(Id id, std::uint32_t tag, const void* raw) {
        if (hold) {
            jobs.emplace_back([this, id, tag, raw]() { fill(id, tag, raw); });
            return nullptr;
        }
        Kind* arch = tabs[id].type;
        Mask mask = arch->mask;
        mask.set(tag);

        move(id, mask);

        void* pointer = peek(id, tag);
        if (raw && pointer) {
            std::memcpy(pointer, raw, bits[tag]);
        }

        auto find = acts.find(tag);
        if (find != acts.end()) {
            find->second(id);
        }
        return pointer;
    }

    void* Zone::peek(Id id, std::uint32_t tag) {
        Slot record = tabs[id];
        Kind* arch = record.type;
        if (!arch) return nullptr;

        auto find = arch->maps.find(tag);
        if (find == arch->maps.end()) return nullptr;

        std::size_t size = bits[tag];
        if (size == 0) return nullptr;
        return arch->page[find->second].data() + record.row * size;
    }

    bool Zone::test(Id id, std::uint32_t tag) const {
        Slot record = tabs[id];
        if (!record.type) return false;
        return record.type->mask.test(tag);
    }

    void Zone::drop(Id id, std::uint32_t tag) {
        if (hold) {
            jobs.emplace_back([this, id, tag]() { drop(id, tag); });
            return;
        }
        Slot record = tabs[id];
        Kind* arch = record.type;
        Mask mask = arch->mask;
        mask.reset(tag);
        move(id, mask);
    }

    void Zone::meta(std::uint32_t tag, const void* raw, std::size_t size) {
        auto& resource = vats[tag];
        resource.resize(size);
        if (raw && size > 0) {
            std::memcpy(resource.data(), raw, size);
        }
    }

    void* Zone::read(std::uint32_t tag) {
        auto find = vats.find(tag);
        if (find == vats.end()) return nullptr;
        return find->second.data();
    }

    void Zone::wire(std::uint32_t tag, const Task& call) {
        acts[tag] = call;
    }

    void Zone::bulk(const std::function<void()>& flow) {
        hold = true;
        flow();
        hold = false;
        for (const auto& task : jobs) {
            task();
        }
        jobs.clear();
    }

    Find Zone::seek() const {
        return Find(pool);
    }

    void Zone::loop(const Find& find, const Find::Call& call) const {
        find.exec(call, find.plan());
    }

    Kind* Zone::view(const Mask& mask) {
        for (const auto& arch : pool) {
            if (arch->mask == mask) return arch.get();
        }
        auto arch = std::make_unique<Kind>();
        arch->mask = mask;
        for (std::uint32_t tag = 0; tag < Cap; ++tag) {
            if (mask.test(tag)) {
                arch->maps[tag] = arch->page.size();
                arch->page.emplace_back();
                arch->bits.push_back(bits[tag]);
            }
        }

        pool.push_back(std::move(arch));
        return pool.back().get();
    }

    void Zone::move(Id id, const Mask& mask) {
        Slot source = tabs[id];
        Kind* old = source.type;
        if (old->mask == mask) return;

        Kind* goal = view(mask);
        goal->push(id);

        Slot destination = {goal, goal->rows.size() - 1};

        for (auto const& [tag, track] : old->maps) {
            if (mask.test(tag)) {
                std::size_t size = bits[tag];
                if (size > 0) {
                    std::size_t offset = goal->maps[tag];
                    std::memcpy(goal->page[offset].data() + destination.row * size,
                                old->page[track].data() + source.row * size,
                                size);
                }
            }
        }

        std::uint32_t moved = old->pull(source.row);
        if (moved != id) {
            tabs[moved].row = source.row;
        }
        tabs[id] = destination;
    }

}