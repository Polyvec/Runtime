#include "../include/core/types/quaternion.hpp"
#include <cassert>
#include <numbers>

namespace core::math {

    const Quaternion Quaternion::IDENTITY = {0.0f, 0.0f, 0.0f, 1.0f};

    Quaternion::Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion::Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    Quaternion Quaternion::operator+(const Quaternion &other) const {
        return {x + other.x, y + other.y, z + other.z, w + other.w};
    }
    Quaternion Quaternion::operator-(const Quaternion &other) const {
        return {x - other.x, y - other.y, z - other.z, w - other.w};
    }
    Quaternion Quaternion::operator*(const Quaternion &other) const {
        return {
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w,
            w * other.w - x * other.x - y * other.y - z * other.z
        };
    }
    Quaternion Quaternion::operator*(float value) const {
        return {x * value, y * value, z * value, w * value};
    }

    Quaternion &Quaternion::operator+=(const Quaternion &other) {
        x += other.x; y += other.y; z += other.z; w += other.w; return *this;
    }
    Quaternion &Quaternion::operator-=(const Quaternion &other) {
        x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this;
    }
    Quaternion &Quaternion::operator*=(const Quaternion &other) {
        *this = *this * other; return *this;
    }
    Quaternion &Quaternion::operator*=(float value) {
        x *= value; y *= value; z *= value; w *= value; return *this;
    }

    bool Quaternion::operator==(const Quaternion &other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }
    bool Quaternion::operator!=(const Quaternion &other) const {
        return !(*this == other);
    }

    Vector3 Quaternion::operator*(const Vector3 &vector) const {
        Vector3 axis = {vector.x, vector.y, vector.z};
        Vector3 first = Vector3(x, y, z).cross(axis);
        Vector3 second = Vector3(x, y, z).cross(first);
        return axis + ((first * w) + second) * 2.0f;
    }

    Scalar Quaternion::dot(const Quaternion &other) const {
        return Scalar(x * other.x + y * other.y + z * other.z + w * other.w);
    }

    Scalar Quaternion::length() const {
        return Scalar(std::sqrt(static_cast<float>(this->dot(*this))));
    }

    Quaternion Quaternion::normalized() const {
        float value = static_cast<float>(this->length());
        assert(value > 0.0f);
        return {x / value, y / value, z / value, w / value};
    }

    Quaternion Quaternion::conjugate() const {
        return {-x, -y, -z, w};
    }

    Quaternion Quaternion::inverse() const {
        float value = static_cast<float>(this->dot(*this));
        assert(value > 0.0f);
        return {-x / value, -y / value, -z / value, w / value};
    }

    Quaternion Quaternion::interpolate(const Quaternion &other, float factor) const {
        float cosine = static_cast<float>(this->dot(other));
        Quaternion target = other;
        if (cosine < 0.0f) {
            cosine = -cosine;
            target = {-other.x, -other.y, -other.z, -other.w};
        }
        if (cosine > 0.9995f) {
            Quaternion result = {
                x + (target.x - x) * factor,
                y + (target.y - y) * factor,
                z + (target.z - z) * factor,
                w + (target.w - w) * factor
            };
            return result.normalized();
        }
        float angle = std::acos(cosine);
        float sine = std::sin(angle);
        float alpha = std::sin((1.0f - factor) * angle) / sine;
        float beta = std::sin(factor * angle) / sine;
        return {
            x * alpha + target.x * beta,
            y * alpha + target.y * beta,
            z * alpha + target.z * beta,
            w * alpha + target.w * beta
        };
    }

    bool Quaternion::approximately(const Quaternion &other, float epsilon) const {
        return std::fabs(x - other.x) <= epsilon &&
               std::fabs(y - other.y) <= epsilon &&
               std::fabs(z - other.z) <= epsilon &&
               std::fabs(w - other.w) <= epsilon;
    }

    Vector3 Quaternion::euler() const {
        Vector3 angles;
        float top = 2.0f * (w * x + y * z);
        float bottom = 1.0f - 2.0f * (x * x + y * y);
        angles.x = std::atan2(top, bottom);

        float value = 2.0f * (w * y - z * x);
        if (std::fabs(value) >= 1.0f)
            angles.y = std::copysign(std::numbers::pi_v<float> / 2.0f, value);
        else
            angles.y = std::asin(value);

        float front = 2.0f * (w * z + x * y);
        float back = 1.0f - 2.0f * (y * y + z * z);
        angles.z = std::atan2(front, back);

        return angles * (180.0f / std::numbers::pi_v<float>);
    }

    Quaternion Quaternion::euler(const Vector3 &degrees) {
        Vector3 radians = degrees * (std::numbers::pi_v<float> / 180.0f);
        float cosine[3] = { std::cos(radians.x * 0.5f), std::cos(radians.y * 0.5f), std::cos(radians.z * 0.5f) };
        float sine[3] = { std::sin(radians.x * 0.5f), std::sin(radians.y * 0.5f), std::sin(radians.z * 0.5f) };

        return {
            sine[0] * cosine[1] * cosine[2] - cosine[0] * sine[1] * sine[2],
            cosine[0] * sine[1] * cosine[2] + sine[0] * cosine[1] * sine[2],
            cosine[0] * cosine[1] * sine[2] - sine[0] * sine[1] * cosine[2],
            cosine[0] * cosine[1] * cosine[2] + sine[0] * sine[1] * sine[2]
        };
    }

    Quaternion Quaternion::between(const Vector3 &from, const Vector3 &to) {
        float cosine = from.dot(to);
        if (cosine < -0.9999f) {
            Vector3 base = {1.0f, 0.0f, 0.0f};
            Vector3 perpendicular = base.cross(from);
            if (perpendicular.length() < 0.001f) {
                Vector3 next = {0.0f, 1.0f, 0.0f};
                perpendicular = next.cross(from);
            }
            return around(180.0f, perpendicular.normalized());
        }
        Vector3 axis = from.cross(to);
        Quaternion result = {axis.x, axis.y, axis.z, 1.0f + cosine};
        return result.normalized();
    }

    Quaternion Quaternion::look(const Vector3 &forward, const Vector3 &up) {
        assert(forward.length() > 0.0f);
        assert(up.length() > 0.0f);
        return between(Vector3::FORWARD, forward.normalized());
    }

    Quaternion Quaternion::around(float angle, const Vector3 &axis) {
        assert(axis.length() > 0.0f);
        float radians = angle * (std::numbers::pi_v<float> / 180.0f);
        float sine = std::sin(radians * 0.5f);
        Vector3 normalized = axis.normalized();
        return {normalized.x * sine, normalized.y * sine, normalized.z * sine, std::cos(radians * 0.5f)};
    }

    std::ostream &operator<<(std::ostream &stream, const Quaternion &other) {
        return stream << "Quaternion(" << other.x << ", " << other.y << ", " << other.z << ", " << other.w << ")";
    }
}