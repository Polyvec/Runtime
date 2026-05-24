#pragma once

#include "vector3.hpp"
#include "quaternion.hpp"
#include "matrix4.hpp"
#include <limits>
#include <ostream>

namespace voxyl::math {

    struct Transform {
        Vector3 translation;
        Quaternion rotation;
        Vector3 scale;

        Transform();
        Transform(const Vector3 &translation, const Quaternion &rotation, const Vector3 &scale);

        Transform operator*(const Transform &transform) const;
        Transform &operator*=(const Transform &transform);

        bool operator==(const Transform &transform) const;
        bool operator!=(const Transform &transform) const;

        [[nodiscard]] Matrix4 matrix() const;

        [[nodiscard]] Vector3 forward() const;
        [[nodiscard]] Vector3 right() const;
        [[nodiscard]] Vector3 up() const;

        [[nodiscard]] Transform inversed() const;
        [[nodiscard]] Transform interpolate(const Transform &transform, float factor) const;

        [[nodiscard]] Vector3 point(const Vector3 &point) const;
        [[nodiscard]] Vector3 direction(const Vector3 &direction) const;

        [[nodiscard]] bool approximately(const Transform &transform, float epsilon = std::numeric_limits<float>::epsilon()) const;

        static Transform from(const Matrix4 &matrix);

        static const Transform IDENTITY;

        friend std::ostream &operator<<(std::ostream &stream, const Transform &transform);
    };

}