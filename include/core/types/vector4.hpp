#pragma once

#include "vector3.hpp"
#include <ostream>

namespace core::math {

    class alignas(16) Vector4 {
        public:
        float x;
        float y;
        float z;
        float w;

        static const Vector4 ZERO;
        static const Vector4 ONE;

        Vector4();
        Vector4(float x, float y, float z, float w);
        Vector4(const Vector3& vector, float w);

        [[nodiscard]] Vector4 operator+(const Vector4& vector) const;
        Vector4& operator+=(const Vector4& vector);
        [[nodiscard]] Vector4 operator-(const Vector4& vector) const;
        Vector4& operator-=(const Vector4& vector);
        [[nodiscard]] Vector4 operator*(float scalar) const;
        Vector4& operator*=(float scalar);
        [[nodiscard]] Vector4 operator/(float scalar) const;
        Vector4& operator/=(float scalar);
        [[nodiscard]] Vector4 operator-() const;

        [[nodiscard]] float dot(const Vector4& vector) const;
        [[nodiscard]] float length() const;
        [[nodiscard]] float squared() const;
        [[nodiscard]] Vector4 normalized() const;
        [[nodiscard]] Vector4 absolute() const;
        [[nodiscard]] Vector4 clamped(const Vector4& minimum, const Vector4& maximum) const;
        [[nodiscard]] bool approximately(const Vector4& vector, float epsilon = 0.0001f) const;

        static Vector4 minimum(const Vector4& alpha, const Vector4& beta);
        static Vector4 maximum(const Vector4& alpha, const Vector4& beta);

        bool operator==(const Vector4& vector) const = default;
    };

    [[nodiscard]] Vector4 operator*(float scalar, const Vector4& vector);

    std::ostream& operator<<(std::ostream& os, const Vector4& vector);

}