#pragma once

#include "vector2.hpp"
#include <ostream>

namespace voxyl::math {

    class Vector3 {
    public:
        float x;
        float y;
        float z;

        static const Vector3 ZERO;
        static const Vector3 ONE;
        static const Vector3 UP;
        static const Vector3 DOWN;
        static const Vector3 LEFT;
        static const Vector3 RIGHT;
        static const Vector3 FORWARD;
        static const Vector3 BACK;

        Vector3();
        Vector3(float x, float y, float z);
        Vector3(const Vector2& vector, float z);

        [[nodiscard]] Vector3 operator+(const Vector3& vector) const;
        Vector3& operator+=(const Vector3& vector);
        [[nodiscard]] Vector3 operator-(const Vector3& vector) const;
        Vector3& operator-=(const Vector3& vector);
        [[nodiscard]] Vector3 operator*(float scalar) const;
        Vector3& operator*=(float scalar);
        [[nodiscard]] Vector3 operator*(const Vector3& vector) const;
        Vector3& operator*=(const Vector3& vector);
        [[nodiscard]] Vector3 operator/(float scalar) const;
        Vector3& operator/=(float scalar);
        [[nodiscard]] Vector3 operator-() const;

        [[nodiscard]] float dot(const Vector3& vector) const;
        [[nodiscard]] Vector3 cross(const Vector3& vector) const;
        [[nodiscard]] float length() const;
        [[nodiscard]] float squared() const;
        [[nodiscard]] Vector3 normalized() const;
        [[nodiscard]] Vector3 absolute() const;
        [[nodiscard]] Vector3 reflect(const Vector3& normal) const;
        [[nodiscard]] Vector3 project(const Vector3& onto) const;
        [[nodiscard]] Vector3 clamped(const Vector3& minimum, const Vector3& maximum) const;
        [[nodiscard]] Vector3 interpolate(const Vector3& vector, float factor) const;
        [[nodiscard]] bool approximately(const Vector3& vector, float epsilon = 0.0001f) const;

        static Vector3 minimum(const Vector3& alpha, const Vector3& beta);
        static Vector3 maximum(const Vector3& alpha, const Vector3& beta);

        bool operator==(const Vector3& vector) const = default;
    };

    std::ostream& operator<<(std::ostream& os, const Vector3& vector);

}