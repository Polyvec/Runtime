#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "kind.hpp"
#include "find.hpp"

namespace core {

    class Zone {
    public:
        Zone() = default;
        ~Zone() = default;

        Zone(const Zone&) = delete;
        Zone& operator=(const Zone&) = delete;
        Zone(Zone&&) noexcept = default;
        Zone& operator=(Zone&&) noexcept = default;

        std::uint32_t type(const std::string& name, std::size_t size = 0);

        Id make();
        Id twin(Id id);
        void kill(Id id);
        void wipe();

        void weld(Id item, Id boss);
        [[nodiscard]] Id boss(Id id) const;
        [[nodiscard]] const std::vector<Id>& crew(Id id) const;

        void* fill(Id id, std::uint32_t tag, const void* raw = nullptr);
        void* peek(Id id, std::uint32_t tag);
        [[nodiscard]] bool test(Id id, std::uint32_t tag) const;
        void drop(Id id, std::uint32_t tag);

        void meta(std::uint32_t tag, const void* raw, std::size_t size);
        void* read(std::uint32_t tag);

        using Task = std::function<void(Id id)>;
        void wire(std::uint32_t tag, const Task& call);

        void bulk(const std::function<void()>& flow);
        [[nodiscard]] Find seek() const;
        void loop(const Find& find, const Find::Call& call) const;

    private:
        Kind* view(const Mask& mask);
        void move(Id id, const Mask& mask);

        std::uint32_t stem = 0xFFFFFFFF;
        std::uint32_t twig = 0xFFFFFFFF;

        std::vector<std::unique_ptr<Kind>> pool;
        std::vector<Slot> tabs;
        std::vector<Id> dead;
        std::vector<std::size_t> bits;
        std::unordered_map<std::string, std::uint32_t> keys;
        std::unordered_map<std::uint32_t, std::vector<std::uint8_t>> vats;
        std::unordered_map<std::uint32_t, Task> acts;
        std::vector<std::function<void()>> jobs;
        bool hold = false;
        Id next = 0;
    };

}