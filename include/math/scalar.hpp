#pragma once

#include <cassert>
#include <numbers>
#include <ostream>

namespace voxyl::math {

    struct Scalar {
        float value;

        Scalar() : value(0.0f) {}
        explicit Scalar(const float value) : value(value) {}

        [[nodiscard]] Scalar squared() const { return Scalar(value * value); }
        [[nodiscard]] Scalar cubed() const { return Scalar(value * value * value); }
        [[nodiscard]] Scalar absolute() const { return Scalar(std::fabs(value)); }
        [[nodiscard]] Scalar sign() const { return Scalar(value < 0.0f ? -1.0f : value > 0.0f ? 1.0f : 0.0f); }
        [[nodiscard]] Scalar floor() const { return Scalar(std::floor(value)); }
        [[nodiscard]] Scalar ceil() const { return Scalar(std::ceil(value)); }
        [[nodiscard]] Scalar round() const { return Scalar(std::round(value)); }
        [[nodiscard]] Scalar sin() const { return Scalar(std::sin(value)); }
        [[nodiscard]] Scalar cos() const { return Scalar(std::cos(value)); }
        [[nodiscard]] Scalar tan() const { return Scalar(std::tan(value)); }
        [[nodiscard]] Scalar exp() const { return Scalar(std::exp(value)); }

        [[nodiscard]] Scalar root() const {
            assert(value >= 0.0f && "Scalar: square root of negative value");
            return Scalar(std::sqrt(value));
        }

        [[nodiscard]] Scalar inverse() const {
            assert(value != 0.0f && "Scalar: inverse of zero");
            return Scalar(1.0f / value);
        }

        [[nodiscard]] Scalar log() const {
            assert(value > 0.0f && "Scalar: log of non-positive value");
            return Scalar(std::log(value));
        }

        [[nodiscard]] Scalar power(const float exp) const {
            assert((value >= 0.0f || std::floor(exp) == exp) && "Scalar: power of negative value with non-integer exponent");
            return Scalar(std::pow(value, exp));
        }

        [[nodiscard]] Scalar snapped(const float step) const {
            assert(step != 0.0f && "Scalar: snap step cannot be zero");
            return Scalar(std::round(value / step) * step);
        }

        [[nodiscard]] Scalar clamped(const float min, const float max) const {
            assert(min <= max && "Scalar: min must be <= max");
            return Scalar(std::fmax(min, std::fmin(value, max)));
        }

        [[nodiscard]] Scalar lerp(const float target, float factor) const {
            factor = std::fmax(0.0f, std::fmin(factor, 1.0f));
            return Scalar(value + (target - value) * factor);
        }

        [[nodiscard]] Scalar mapped(const float alpha, const float beta, const float origin, const float destination) const {
            assert(beta != alpha && "Scalar: source range cannot be zero");
            return Scalar((value - alpha) / (beta - alpha) * (destination - origin) + origin);
        }

        [[nodiscard]] bool zero() const { return value == 0.0f; }
        [[nodiscard]] bool positive() const { return value > 0.0f; }
        [[nodiscard]] bool negative() const { return value < 0.0f; }
        [[nodiscard]] bool approximately(const float v, const float epsilon = 1e-5f) const { return std::fabs(value - v) <= epsilon; }

        bool operator==(const Scalar &scalar) const { return value == scalar.value; }
        bool operator!=(const Scalar &scalar) const { return value != scalar.value; }
        bool operator<(const Scalar &scalar) const { return value < scalar.value; }
        bool operator>(const Scalar &scalar) const { return value > scalar.value; }
        bool operator<=(const Scalar &scalar) const { return value <= scalar.value; }
        bool operator>=(const Scalar &scalar) const { return value >= scalar.value; }

        Scalar operator+(const Scalar &scalar) const { return Scalar(value + scalar.value); }
        Scalar operator-(const Scalar &scalar) const { return Scalar(value - scalar.value); }
        Scalar operator*(const Scalar &scalar) const { return Scalar(value * scalar.value); }
        Scalar operator-() const { return Scalar(-value); }

        Scalar operator/(const Scalar &scalar) const {
            assert(scalar.value != 0.0f && "Scalar: division by zero");
            return Scalar(value / scalar.value);
        }

        Scalar &operator+=(const Scalar &scalar) { value += scalar.value; return *this; }
        Scalar &operator-=(const Scalar &scalar) { value -= scalar.value; return *this; }
        Scalar &operator*=(const Scalar &scalar) { value *= scalar.value; return *this; }

        Scalar &operator/=(const Scalar &scalar) {
            assert(scalar.value != 0.0f && "Scalar: division by zero");
            value /= scalar.value;
            return *this;
        }

        explicit operator float() const { return value; }

        static Scalar pi() { return Scalar(std::numbers::pi_v<float>); }
        static Scalar tau() { return Scalar(std::numbers::pi_v<float> * 2.0f); }
        static Scalar exponential() { return Scalar(std::numbers::e_v<float>); }

        static Scalar minimum(const Scalar &alpha, const Scalar &beta) { return alpha < beta ? alpha : beta; }
        static Scalar maximum(const Scalar &alpha, const Scalar &beta) { return alpha > beta ? alpha : beta; }

        friend std::ostream &operator<<(std::ostream &os, const Scalar &scalar);
    };

}