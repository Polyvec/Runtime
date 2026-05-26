#pragma once
#include <cstddef>

namespace voxyl::ecs {

    class Archetype;

    using Entity = std::uint32_t;
    constexpr Entity NONE = 0xFFFFFFFF;

    struct Record {
        Archetype* archetype = nullptr;
        std::size_t index = 0;
    };

}