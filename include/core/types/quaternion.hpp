#pragma once

#include "scalar.hpp"
#include "vector3.hpp"
#include <limits>
#include <ostream>

namespace core::math {

    struct Quaternion {
        float x, y, z, w;

        Quaternion();
        Quaternion(float x, float y, float z, float w);

        Quaternion operator+(const Quaternion &quaternion) const;
        Quaternion operator-(const Quaternion &quaternion) const;
        Quaternion operator*(const Quaternion &quaternion) const;
        Quaternion operator*(float scalar) const;

        Quaternion &operator+=(const Quaternion &quaternion);
        Quaternion &operator-=(const Quaternion &quaternion);
        Quaternion &operator*=(const Quaternion &quaternion);
        Quaternion &operator*=(float scalar);

        bool operator==(const Quaternion &quaternion) const;
        bool operator!=(const Quaternion &quaternion) const;

        Vector3 operator*(const Vector3 &vector) const;

        [[nodiscard]] Scalar length() const;
        [[nodiscard]] Scalar dot(const Quaternion &quaternion) const;

        [[nodiscard]] Quaternion normalized() const;
        [[nodiscard]] Quaternion conjugate() const;
        [[nodiscard]] Quaternion inverse() const;
        [[nodiscard]] Quaternion interpolate(const Quaternion &quaternion, float factor) const;

        [[nodiscard]] bool approximately(const Quaternion &quaternion, float epsilon = std::numeric_limits<float>::epsilon()) const;

        [[nodiscard]] Vector3 euler() const;

        static Quaternion euler(const Vector3 &degrees);
        static Quaternion between(const Vector3 &from, const Vector3 &to);
        static Quaternion look(const Vector3 &forward, const Vector3 &up = Vector3::UP);
        static Quaternion around(float angle, const Vector3 &axis);

        static const Quaternion IDENTITY;

        friend std::ostream &operator<<(std::ostream &os, const Quaternion &quaternion);
    };

    inline Quaternion operator*(const float scalar, const Quaternion &quaternion) {
        return quaternion * scalar;
    }

}