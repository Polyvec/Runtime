#pragma once
#include <bitset>
#include <cstddef>

namespace voxyl::ecs {

    constexpr std::size_t MAX_COMPONENTS = 256;
    using Mask = std::bitset<MAX_COMPONENTS>;

}
