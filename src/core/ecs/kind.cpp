#include "../include/core/ecs/kind.hpp"
#include <algorithm>

namespace core {

    void Kind::push(Id id) {
        std::unique_lock lock(this->lock);
        rows.push_back(id);
        for (std::size_t index = 0; index < page.size(); ++index) {
            page[index].resize(rows.size() * bits[index]);
        }
        sync.fetch_add(1, std::memory_order_relaxed);
    }

    std::uint32_t Kind::pull(std::size_t row) {
        std::unique_lock lock(this->lock);
        Id last = rows.back();

        for (std::size_t track = 0; track < page.size(); ++track) {
            std::size_t size = bits[track];
            if (size > 0) {
                std::uint8_t* source = page[track].data();
                std::copy(source + (rows.size() - 1) * size,
                          source + rows.size() * size,
                          source + row * size);
                page[track].resize((rows.size() - 1) * size);
            }
        }

        rows[row] = last;
        rows.pop_back();
        sync.fetch_add(1, std::memory_order_relaxed);
        return last;
    }

}