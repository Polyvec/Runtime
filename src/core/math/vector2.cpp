#include "../include/core/math/vector2.hpp"
#include <cassert>

namespace voxyl::math {

    const Vector2 Vector2::ZERO = {0.0f, 0.0f};
    const Vector2 Vector2::ONE = {1.0f, 1.0f};
    const Vector2 Vector2::UP = {0.0f, 1.0f};
    const Vector2 Vector2::DOWN = {0.0f, -1.0f};
    const Vector2 Vector2::LEFT = {-1.0f, 0.0f};
    const Vector2 Vector2::RIGHT = {1.0f, 0.0f};

    Vector2::Vector2() : x(0.0f), y(0.0f) {}
    Vector2::Vector2(float x, float y) : x(x), y(y) {}

    Vector2 Vector2::operator+(const Vector2& vector) const { return {x + vector.x, y + vector.y}; }
    Vector2& Vector2::operator+=(const Vector2& vector) { x += vector.x; y += vector.y; return *this; }
    Vector2 Vector2::operator-(const Vector2& vector) const { return {x - vector.x, y - vector.y}; }
    Vector2& Vector2::operator-=(const Vector2& vector) { x -= vector.x; y -= vector.y; return *this; }
    Vector2 Vector2::operator*(float scalar) const { return {x * scalar, y * scalar}; }
    Vector2& Vector2::operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    Vector2 operator*(float scalar, const Vector2& vector) { return vector * scalar; }
    Vector2 Vector2::operator/(float scalar) const { assert(scalar != 0.0f); return {x / scalar, y / scalar}; }
    Vector2& Vector2::operator/=(float scalar) { assert(scalar != 0.0f); x /= scalar; y /= scalar; return *this; }
    Vector2 Vector2::operator-() const { return {-x, -y}; }

    float Vector2::dot(const Vector2& vector) const { return x * vector.x + y * vector.y; }
    float Vector2::cross(const Vector2& vector) const { return x * vector.y - y * vector.x; }
    float Vector2::length() const { return std::sqrt(x * x + y * y); }
    float Vector2::squared() const { return x * x + y * y; }
    Vector2 Vector2::normalized() const { float len = length(); assert(len > 0.0f); return *this / len; }
    Vector2 Vector2::absolute() const { return {std::fabs(x), std::fabs(y)}; }
    Vector2 Vector2::perpendicular() const { return {-y, x}; }
    Vector2 Vector2::reflect(const Vector2& normal) const { return *this - normal * (2.0f * this->dot(normal)); }
    Vector2 Vector2::project(const Vector2& onto) const { float sq = onto.squared(); assert(sq > 0.0f); return onto * (this->dot(onto) / sq); }

    Vector2 Vector2::clamped(const Vector2& minimum, const Vector2& maximum) const {
        return {std::fmax(minimum.x, std::fmin(x, maximum.x)), std::fmax(minimum.y, std::fmin(y, maximum.y))};
    }

    bool Vector2::approximately(const Vector2& vector, float epsilon) const {
        return std::fabs(x - vector.x) <= epsilon && std::fabs(y - vector.y) <= epsilon;
    }

    Vector2 Vector2::minimum(const Vector2& alpha, const Vector2& beta) {
        return {std::fmin(alpha.x, beta.x), std::fmin(alpha.y, beta.y)};
    }
    Vector2 Vector2::maximum(const Vector2& alpha, const Vector2& beta) {
        return {std::fmax(alpha.x, beta.x), std::fmax(alpha.y, beta.y)};
    }

    std::ostream& operator<<(std::ostream& os, const Vector2& vector) {
        return os << "(" << vector.x << ", " << vector.y << ")";
    }
}