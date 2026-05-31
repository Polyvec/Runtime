#include "../include/core/types/scalar.hpp"

namespace core::math {

    std::ostream &operator<<(std::ostream &os, const Scalar &scalar) {
        return os << scalar.value;
    }

}