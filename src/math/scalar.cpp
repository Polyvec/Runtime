#include "../include/math/scalar.hpp"

namespace voxyl::math {

    std::ostream &operator<<(std::ostream &os, const Scalar &scalar) {
        return os << scalar.value;
    }

}