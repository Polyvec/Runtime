#include "../include/core/math/vector3.hpp"
#include <cassert>

namespace voxyl::math {

    const Vector3 Vector3::ZERO = {0.0f, 0.0f, 0.0f};
    const Vector3 Vector3::ONE = {1.0f, 1.0f, 1.0f};
    const Vector3 Vector3::UP = {0.0f, 1.0f, 0.0f};
    const Vector3 Vector3::DOWN = {0.0f, -1.0f, 0.0f};
    const Vector3 Vector3::LEFT = {-1.0f, 0.0f, 0.0f};
    const Vector3 Vector3::RIGHT = {1.0f, 0.0f, 0.0f};
    const Vector3 Vector3::FORWARD = {0.0f, 0.0f, 1.0f};
    const Vector3 Vector3::BACK = {0.0f, 0.0f, -1.0f};

    Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector3::Vector3(const Vector2& vector, float z) : x(vector.x), y(vector.y), z(z) {}

    Vector3 Vector3::operator+(const Vector3& vector) const { return {x + vector.x, y + vector.y, z + vector.z}; }
    Vector3& Vector3::operator+=(const Vector3& vector) { x += vector.x; y += vector.y; z += vector.z; return *this; }
    Vector3 Vector3::operator-(const Vector3& vector) const { return {x - vector.x, y - vector.y, z - vector.z}; }
    Vector3& Vector3::operator-=(const Vector3& vector) { x -= vector.x; y -= vector.y; z -= vector.z; return *this; }

    Vector3 Vector3::operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }
    Vector3& Vector3::operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vector3 Vector3::operator*(const Vector3& vector) const { return {x * vector.x, y * vector.y, z * vector.z}; }
    Vector3& Vector3::operator*=(const Vector3& vector) { x *= vector.x; y *= vector.y; z *= vector.z; return *this; }

    Vector3 Vector3::operator/(float scalar) const { assert(scalar != 0.0f); return {x / scalar, y / scalar, z / scalar}; }
    Vector3& Vector3::operator/=(float scalar) { assert(scalar != 0.0f); x /= scalar; y /= scalar; z /= scalar; return *this; }
    Vector3 Vector3::operator-() const { return {-x, -y, -z}; }

    float Vector3::dot(const Vector3& vector) const { return x * vector.x + y * vector.y + z * vector.z; }
    Vector3 Vector3::cross(const Vector3& vector) const {
        return {
            y * vector.z - z * vector.y,
            z * vector.x - x * vector.z,
            x * vector.y - y * vector.x
        };
    }
    float Vector3::length() const { return std::sqrt(x * x + y * y + z * z); }
    float Vector3::squared() const { return x * x + y * y + z * z; }
    Vector3 Vector3::normalized() const { float len = length(); assert(len > 0.0f); return *this / len; }
    Vector3 Vector3::absolute() const { return {std::fabs(x), std::fabs(y), std::fabs(z)}; }
    Vector3 Vector3::reflect(const Vector3& normal) const { return *this - normal * (2.0f * this->dot(normal)); }
    Vector3 Vector3::project(const Vector3& onto) const { float sq = onto.squared(); assert(sq > 0.0f); return onto * (this->dot(onto) / sq); }

    Vector3 Vector3::clamped(const Vector3& minimum, const Vector3& maximum) const {
        return {
            std::fmax(minimum.x, std::fmin(x, maximum.x)),
            std::fmax(minimum.y, std::fmin(y, maximum.y)),
            std::fmax(minimum.z, std::fmin(z, maximum.z))
        };
    }

    Vector3 Vector3::interpolate(const Vector3& vector, float factor) const {
        return {
            (1.0f - factor) * x + factor * vector.x,
            (1.0f - factor) * y + factor * vector.y,
            (1.0f - factor) * z + factor * vector.z
        };
    }

    bool Vector3::approximately(const Vector3& vector, float epsilon) const {
        return std::fabs(x - vector.x) <= epsilon && std::fabs(y - vector.y) <= epsilon && std::fabs(z - vector.z) <= epsilon;
    }

    Vector3 Vector3::minimum(const Vector3& alpha, const Vector3& beta) {
        return {std::fmin(alpha.x, beta.x), std::fmin(alpha.y, beta.y), std::fmin(alpha.z, beta.z)};
    }
    Vector3 Vector3::maximum(const Vector3& alpha, const Vector3& beta) {
        return {std::fmax(alpha.x, beta.x), std::fmax(alpha.y, beta.y), std::fmax(alpha.z, beta.z)};
    }

    std::ostream& operator<<(std::ostream& os, const Vector3& vector) {
        return os << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
    }
}