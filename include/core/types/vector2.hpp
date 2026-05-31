#pragma once

#include <ostream>

namespace core::math {

    class Vector2 {
    public:
        float x;
        float y;

        static const Vector2 ZERO;
        static const Vector2 ONE;
        static const Vector2 UP;
        static const Vector2 DOWN;
        static const Vector2 LEFT;
        static const Vector2 RIGHT;

        Vector2();
        Vector2(float x, float y);

        [[nodiscard]] Vector2 operator+(const Vector2& vector) const;
        Vector2& operator+=(const Vector2& vector);
        [[nodiscard]] Vector2 operator-(const Vector2& vector) const;
        Vector2& operator-=(const Vector2& vector);
        [[nodiscard]] Vector2 operator*(float scalar) const;
        Vector2& operator*=(float scalar);
        [[nodiscard]] Vector2 operator/(float scalar) const;
        Vector2& operator/=(float scalar);
        [[nodiscard]] Vector2 operator-() const;

        [[nodiscard]] float dot(const Vector2& vector) const;
        [[nodiscard]] float cross(const Vector2& vector) const;
        [[nodiscard]] float length() const;
        [[nodiscard]] float squared() const;
        [[nodiscard]] Vector2 normalized() const;
        [[nodiscard]] Vector2 absolute() const;
        [[nodiscard]] Vector2 perpendicular() const;
        [[nodiscard]] Vector2 reflect(const Vector2& normal) const;
        [[nodiscard]] Vector2 project(const Vector2& onto) const;
        [[nodiscard]] Vector2 clamped(const Vector2& minimum, const Vector2& maximum) const;
        [[nodiscard]] bool approximately(const Vector2& vector, float epsilon = 0.0001f) const;

        static Vector2 minimum(const Vector2& alpha, const Vector2& beta);
        static Vector2 maximum(const Vector2& alpha, const Vector2& beta);

        bool operator==(const Vector2& vector) const = default;
    };

    [[nodiscard]] Vector2 operator*(float scalar, const Vector2& vector);

    std::ostream& operator<<(std::ostream& os, const Vector2& vector);

}