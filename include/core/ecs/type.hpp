#pragma once
#include <cstddef>
#include <cstdint>

namespace core {

    class Kind;

    using Id = std::uint32_t;
    constexpr Id Null = 0xFFFFFFFF;

    struct Slot {
        Kind* type = nullptr;
        std::size_t row = 0;
    };

}