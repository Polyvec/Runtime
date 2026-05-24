#include "../include/math/quaternion.hpp"
#include <cassert>
#include <numbers>

namespace voxyl::math {

    const Quaternion Quaternion::IDENTITY(0.0f, 0.0f, 0.0f, 1.0f);

    Quaternion::Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion::Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    Quaternion Quaternion::operator+(const Quaternion &quaternion) const {
        return Quaternion(x + quaternion.x, y + quaternion.y, z + quaternion.z, w + quaternion.w);
    }
    Quaternion Quaternion::operator-(const Quaternion &quaternion) const {
        return Quaternion(x - quaternion.x, y - quaternion.y, z - quaternion.z, w - quaternion.w);
    }
    Quaternion Quaternion::operator*(const Quaternion &quaternion) const {
        return Quaternion(
            w * quaternion.x + x * quaternion.w + y * quaternion.z - z * quaternion.y,
            w * quaternion.y - x * quaternion.z + y * quaternion.w + z * quaternion.x,
            w * quaternion.z + x * quaternion.y - y * quaternion.x + z * quaternion.w,
            w * quaternion.w - x * quaternion.x - y * quaternion.y - z * quaternion.z
        );
    }
    Quaternion Quaternion::operator*(float scalar) const {
        return Quaternion(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    Quaternion &Quaternion::operator+=(const Quaternion &quaternion) {
        x += quaternion.x; y += quaternion.y; z += quaternion.z; w += quaternion.w; return *this;
    }
    Quaternion &Quaternion::operator-=(const Quaternion &quaternion) {
        x -= quaternion.x; y -= quaternion.y; z -= quaternion.z; w -= quaternion.w; return *this;
    }
    Quaternion &Quaternion::operator*=(const Quaternion &quaternion) {
        *this = *this * quaternion; return *this;
    }
    Quaternion &Quaternion::operator*=(float scalar) {
        x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this;
    }

    bool Quaternion::operator==(const Quaternion &quaternion) const {
        return x == quaternion.x && y == quaternion.y && z == quaternion.z && w == quaternion.w;
    }
    bool Quaternion::operator!=(const Quaternion &quaternion) const {
        return !(*this == quaternion);
    }

    Vector3 Quaternion::operator*(const Vector3 &vector) const {
        Vector3 qv(x, y, z);
        Vector3 uv = qv.cross(vector);
        Vector3 uuv = qv.cross(uv);
        return vector + ((uv * w) + uuv) * 2.0f;
    }

    Scalar Quaternion::length() const {
        return Scalar(std::sqrt(x * x + y * y + z * z + w * w));
    }
    Scalar Quaternion::dot(const Quaternion &quaternion) const {
        return Scalar(x * quaternion.x + y * quaternion.y + z * quaternion.z + w * quaternion.w);
    }

    Quaternion Quaternion::normalized() const {
        float len = std::sqrt(x * x + y * y + z * z + w * w);
        assert(len > 0.0f);
        return Quaternion(x / len, y / len, z / len, w / len);
    }
    Quaternion Quaternion::conjugate() const {
        return Quaternion(-x, -y, -z, w);
    }
    Quaternion Quaternion::inverse() const {
        float norm = x * x + y * y + z * z + w * w;
        assert(norm > 0.0f);
        return Quaternion(-x / norm, -y / norm, -z / norm, w / norm);
    }
    Quaternion Quaternion::interpolate(const Quaternion &quaternion, float factor) const {
        float cosTheta = x * quaternion.x + y * quaternion.y + z * quaternion.z + w * quaternion.w;
        Quaternion target = quaternion;
        if (cosTheta < 0.0f) {
            cosTheta = -cosTheta;
            target = Quaternion(-quaternion.x, -quaternion.y, -quaternion.z, -quaternion.w);
        }
        if (cosTheta > 0.9995f) {
            return Quaternion(
                x + (target.x - x) * factor,
                y + (target.y - y) * factor,
                z + (target.z - z) * factor,
                w + (target.w - w) * factor
            ).normalized();
        }
        float theta = std::acos(cosTheta);
        float sinTheta = std::sin(theta);
        float af = std::sin((1.0f - factor) * theta) / sinTheta;
        float bf = std::sin(factor * theta) / sinTheta;
        return Quaternion(
            x * af + target.x * bf,
            y * af + target.y * bf,
            z * af + target.z * bf,
            w * af + target.w * bf
        );
    }

    bool Quaternion::approximately(const Quaternion &quaternion, float epsilon) const {
        return std::fabs(x - quaternion.x) <= epsilon &&
               std::fabs(y - quaternion.y) <= epsilon &&
               std::fabs(z - quaternion.z) <= epsilon &&
               std::fabs(w - quaternion.w) <= epsilon;
    }

    Vector3 Quaternion::euler() const {
        Vector3 angles;
        float sinr_cosp = 2.0f * (w * x + y * z);
        float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
        angles.x = std::atan2(sinr_cosp, cosr_cosp);

        float sinp = 2.0f * (w * y - z * x);
        if (std::fabs(sinp) >= 1.0f)
            angles.y = std::copysign(std::numbers::pi_v<float> / 2.0f, sinp);
        else
            angles.y = std::asin(sinp);

        float siny_cosp = 2.0f * (w * z + x * y);
        float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
        angles.z = std::atan2(siny_cosp, cosy_cosp);

        return angles * (180.0f / std::numbers::pi_v<float>);
    }

    Quaternion Quaternion::euler(const Vector3 &degrees) {
        Vector3 rad = degrees * (std::numbers::pi_v<float> / 180.0f);
        float cx = std::cos(rad.x * 0.5f);
        float sx = std::sin(rad.x * 0.5f);
        float cy = std::cos(rad.y * 0.5f);
        float sy = std::sin(rad.y * 0.5f);
        float cz = std::cos(rad.z * 0.5f);
        float sz = std::sin(rad.z * 0.5f);

        return Quaternion(
            sx * cy * cz - cx * sy * sz,
            cx * sy * cz + sx * cy * sz,
            cx * cy * sz - sx * sy * cz,
            cx * cy * cz + sx * sy * sz
        );
    }

    Quaternion Quaternion::between(const Vector3 &from, const Vector3 &to) {
        float cosine = from.dot(to);
        if (cosine < -0.9999f) {
            Vector3 perpendicular = Vector3(1.0f, 0.0f, 0.0f).cross(from);
            if (perpendicular.length() < 0.001f)
                perpendicular = Vector3(0.0f, 1.0f, 0.0f).cross(from);
            return around(180.0f, perpendicular.normalized());
        }
        Vector3 axis = from.cross(to);
        return Quaternion(axis.x, axis.y, axis.z, 1.0f + cosine).normalized();
    }

    Quaternion Quaternion::look(const Vector3 &forward, const Vector3 &up) {
        assert(forward.length() > 0.0f);
        assert(up.length() > 0.0f);
        return between(Vector3::FORWARD, forward.normalized());
    }

    Quaternion Quaternion::around(float angle, const Vector3 &axis) {
        assert(axis.length() > 0.0f);
        float radians = angle * (std::numbers::pi_v<float> / 180.0f);
        float sinHalf = std::sin(radians * 0.5f);
        Vector3 normAxis = axis.normalized();
        return Quaternion(normAxis.x * sinHalf, normAxis.y * sinHalf, normAxis.z * sinHalf, std::cos(radians * 0.5f));
    }

    std::ostream &operator<<(std::ostream &os, const Quaternion &quaternion) {
        return os << "Quaternion(" << quaternion.x << ", " << quaternion.y << ", " << quaternion.z << ", " << quaternion.w << ")";
    }
}