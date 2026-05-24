#pragma once

#include "vector3.hpp"
#include "vector4.hpp"
#include "quaternion.hpp"

namespace voxyl::math {

    class alignas(16) Matrix4 {
        public:
        float matrix[4][4]{};

        static const Matrix4 IDENTITY;

        Matrix4();
        explicit Matrix4(float diagonal);

        [[nodiscard]] float operator()(int row, int column) const;
        [[nodiscard]] float& operator()(int row, int column);

        [[nodiscard]] Matrix4 operator+(const Matrix4& other) const;
        [[nodiscard]] Matrix4 operator-(const Matrix4& other) const;
        [[nodiscard]] Matrix4 operator*(const Matrix4& other) const;
        [[nodiscard]] Vector4 operator*(const Vector4& vector) const;

        [[nodiscard]] Matrix4 transposed() const;
        [[nodiscard]] Matrix4 inverted() const;

        static Matrix4 identity();
        static Matrix4 translate(const Vector3& translation);
        static Matrix4 rotate(const Quaternion& rotation);
        static Matrix4 scale(const Vector3& factors);
        static Matrix4 orthographic(float left, float right, float bottom, float top, float zNear, float zFar);
        static Matrix4 perspective(float fov, float aspect, float zNear, float zFar);
        static Matrix4 look(const Vector3& eye, const Vector3& target, const Vector3& up);

        bool decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const;
    };

}