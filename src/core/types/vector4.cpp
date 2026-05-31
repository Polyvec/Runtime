#include "../include/core/types/vector4.hpp"
#include <cassert>

namespace core::math {

    const Vector4 Vector4::ZERO = {0.0f, 0.0f, 0.0f, 0.0f};
    const Vector4 Vector4::ONE = {1.0f, 1.0f, 1.0f, 1.0f};

    Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4::Vector4(const Vector3& vector, float w) : x(vector.x), y(vector.y), z(vector.z), w(w) {}

    Vector4 Vector4::operator+(const Vector4& vector) const { return {x + vector.x, y + vector.y, z + vector.z, w + vector.w}; }
    Vector4& Vector4::operator+=(const Vector4& vector) { x += vector.x; y += vector.y; z += vector.z; w += vector.w; return *this; }
    Vector4 Vector4::operator-(const Vector4& vector) const { return {x - vector.x, y - vector.y, z - vector.z, w - vector.w}; }
    Vector4& Vector4::operator-=(const Vector4& vector) { x -= vector.x; y -= vector.y; z -= vector.z; w -= vector.w; return *this; }
    Vector4 Vector4::operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar, w * scalar}; }
    Vector4& Vector4::operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
    Vector4 operator*(float scalar, const Vector4& vector) { return vector * scalar; }
    Vector4 Vector4::operator/(float scalar) const { assert(scalar != 0.0f); return {x / scalar, y / scalar, z / scalar, w / scalar}; }
    Vector4& Vector4::operator/=(float scalar) { assert(scalar != 0.0f); x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }
    Vector4 Vector4::operator-() const { return {-x, -y, -z, -w}; }

    float Vector4::dot(const Vector4& vector) const { return x * vector.x + y * vector.y + z * vector.z + w * vector.w; }
    float Vector4::length() const { return std::sqrt(x * x + y * y + z * z + w * w); }
    float Vector4::squared() const { return x * x + y * y + z * z + w * w; }
    Vector4 Vector4::normalized() const { float len = length(); assert(len > 0.0f); return *this / len; }
    Vector4 Vector4::absolute() const { return {std::fabs(x), std::fabs(y), std::fabs(z), std::fabs(w)}; }

    Vector4 Vector4::clamped(const Vector4& minimum, const Vector4& maximum) const {
        return {
            std::fmax(minimum.x, std::fmin(x, maximum.x)),
            std::fmax(minimum.y, std::fmin(y, maximum.y)),
            std::fmax(minimum.z, std::fmin(z, maximum.z)),
            std::fmax(minimum.w, std::fmin(w, maximum.w))
        };
    }

    bool Vector4::approximately(const Vector4& vector, float epsilon) const {
        return std::fabs(x - vector.x) <= epsilon && std::fabs(y - vector.y) <= epsilon && std::fabs(z - vector.z) <= epsilon && std::fabs(w - vector.w) <= epsilon;
    }

    Vector4 Vector4::minimum(const Vector4& alpha, const Vector4& beta) {
        return {std::fmin(alpha.x, beta.x), std::fmin(alpha.y, beta.y), std::fmin(alpha.z, beta.z), std::fmin(alpha.w, beta.w)};
    }
    Vector4 Vector4::maximum(const Vector4& alpha, const Vector4& beta) {
        return {std::fmax(alpha.x, beta.x), std::fmax(alpha.y, beta.y), std::fmax(alpha.z, beta.z), std::fmax(alpha.w, beta.w)};
    }

    std::ostream& operator<<(std::ostream& os, const Vector4& vector) {
        return os << "(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";
    }
}