#pragma once

#include "scalar.hpp"
#include "vector2.hpp"
#include <limits>
#include <ostream>

namespace core::math {

    struct Matrix3 {
        float matrix[3][3]{};

        Matrix3() = default;

        explicit Matrix3(const float scalar) {
            matrix[0][0] = scalar;
            matrix[1][1] = scalar;
            matrix[2][2] = scalar;
        }

        float operator()(int row, int column) const;
        float &operator()(int row, int column);

        Matrix3 operator+(const Matrix3 &other) const;
        Matrix3 operator-(const Matrix3 &other) const;
        Matrix3 operator*(const Matrix3 &other) const;
        Matrix3 operator*(float scalar) const;
        Vector2 operator*(const Vector2 &vector) const;

        Matrix3 &operator+=(const Matrix3 &other);
        Matrix3 &operator-=(const Matrix3 &other);
        Matrix3 &operator*=(const Matrix3 &other);
        Matrix3 &operator*=(float scalar);

        bool operator==(const Matrix3 &other) const;
        bool operator!=(const Matrix3 &other) const;

        [[nodiscard]] Matrix3 transposed() const;
        [[nodiscard]] Matrix3 inverted() const;
        [[nodiscard]] Scalar determinant() const;

        [[nodiscard]] bool approximately(const Matrix3 &other, float epsilon = std::numeric_limits<float>::epsilon()) const;

        [[nodiscard]] Vector2 translation() const;
        [[nodiscard]] Scalar rotation() const;
        [[nodiscard]] Vector2 scale() const;

        [[nodiscard]] Vector2 superior() const;
        [[nodiscard]] Vector2 inferior() const;
        [[nodiscard]] Vector2 dexter() const;
        [[nodiscard]] Vector2 sinister() const;

        [[nodiscard]] bool decompose(Vector2 &translation, float &rotation, Vector2 &scale) const;

        static Matrix3 identity();
        static Matrix3 translate(const Vector2 &translation);
        static Matrix3 rotate(float angle);
        static Matrix3 scale(const Vector2 &scale);
        static Matrix3 compose(const Vector2 &translation, float rotation, const Vector2 &scale);
        static Matrix3 orthographic(float left, float right, float bottom, float top);

        static const Matrix3 IDENTITY;

        friend std::ostream &operator<<(std::ostream &os, const Matrix3 &other);
    };

    inline Matrix3 operator*(float scalar, const Matrix3 &other) {
        return other * scalar;
    }

}