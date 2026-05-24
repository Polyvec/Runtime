#include "../include/math/matrix4.hpp"
#include <cassert>

namespace voxyl::math {

    const Matrix4 Matrix4::IDENTITY = Matrix4(1.0f);

    Matrix4::Matrix4() {
        for (int row = 0; row < 4; row++) {
            for (int column = 0; column < 4; column++) {
                matrix[row][column] = 0.0f;
            }
        }
    }

    Matrix4::Matrix4(float diagonal) : Matrix4() {
        matrix[0][0] = diagonal;
        matrix[1][1] = diagonal;
        matrix[2][2] = diagonal;
        matrix[3][3] = diagonal;
    }

    float Matrix4::operator()(int row, int column) const {
        assert(row >= 0 && row < 4);
        assert(column >= 0 && column < 4);
        return this->matrix[row][column];
    }

    float& Matrix4::operator()(int row, int column) {
        assert(row >= 0 && row < 4);
        assert(column >= 0 && column < 4);
        return this->matrix[row][column];
    }

    Matrix4 Matrix4::operator+(const Matrix4& other) const {
        Matrix4 result;
        for (int row = 0; row < 4; row++) {
            for (int column = 0; column < 4; column++) {
                result.matrix[row][column] = this->matrix[row][column] + other.matrix[row][column];
            }
        }
        return result;
    }

    Matrix4 Matrix4::operator-(const Matrix4& other) const {
        Matrix4 result;
        for (int row = 0; row < 4; row++) {
            for (int column = 0; column < 4; column++) {
                result.matrix[row][column] = this->matrix[row][column] - other.matrix[row][column];
            }
        }
        return result;
    }

    Matrix4 Matrix4::operator*(const Matrix4& other) const {
        Matrix4 result;
        for (int row = 0; row < 4; row++) {
            for (int column = 0; column < 4; column++) {
                result.matrix[row][column] =
                    this->matrix[row][0] * other.matrix[0][column] +
                    this->matrix[row][1] * other.matrix[1][column] +
                    this->matrix[row][2] * other.matrix[2][column] +
                    this->matrix[row][3] * other.matrix[3][column];
            }
        }
        return result;
    }

    Vector4 Matrix4::operator*(const Vector4& vector) const {
        return Vector4(
            this->matrix[0][0] * vector.x + this->matrix[0][1] * vector.y + this->matrix[0][2] * vector.z + this->matrix[0][3] * vector.w,
            this->matrix[1][0] * vector.x + this->matrix[1][1] * vector.y + this->matrix[1][2] * vector.z + this->matrix[1][3] * vector.w,
            this->matrix[2][0] * vector.x + this->matrix[2][1] * vector.y + this->matrix[2][2] * vector.z + this->matrix[2][3] * vector.w,
            this->matrix[3][0] * vector.x + this->matrix[3][1] * vector.y + this->matrix[3][2] * vector.z + this->matrix[3][3] * vector.w
        );
    }

    Matrix4 Matrix4::transposed() const {
        Matrix4 result;
        for (int row = 0; row < 4; row++) {
            for (int column = 0; column < 4; column++) {
                result.matrix[column][row] = this->matrix[row][column];
            }
        }
        return result;
    }

    Matrix4 Matrix4::inverted() const {
        float s0 = matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
        float s1 = matrix[0][0] * matrix[1][2] - matrix[0][2] * matrix[1][0];
        float s2 = matrix[0][0] * matrix[1][3] - matrix[0][3] * matrix[1][0];
        float s3 = matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1];
        float s4 = matrix[0][1] * matrix[1][3] - matrix[0][3] * matrix[1][1];
        float s5 = matrix[0][2] * matrix[1][3] - matrix[0][3] * matrix[1][2];

        float c0 = matrix[2][0] * matrix[3][1] - matrix[2][1] * matrix[3][0];
        float c1 = matrix[2][0] * matrix[3][2] - matrix[2][2] * matrix[3][0];
        float c2 = matrix[2][0] * matrix[3][3] - matrix[2][3] * matrix[3][0];
        float c3 = matrix[2][1] * matrix[3][2] - matrix[2][2] * matrix[3][1];
        float c4 = matrix[2][1] * matrix[3][3] - matrix[2][3] * matrix[3][1];
        float c5 = matrix[2][2] * matrix[3][3] - matrix[2][3] * matrix[3][2];

        float det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
        assert(det != 0.0f);
        float invDet = 1.0f / det;

        Matrix4 inv;
        inv.matrix[0][0] = (matrix[1][1] * c5 - matrix[1][2] * c4 + matrix[1][3] * c3) * invDet;
        inv.matrix[0][1] = (-matrix[0][1] * c5 + matrix[0][2] * c4 - matrix[0][3] * c3) * invDet;
        inv.matrix[0][2] = (matrix[3][1] * s5 - matrix[3][2] * s4 + matrix[3][3] * s3) * invDet;
        inv.matrix[0][3] = (-matrix[2][1] * s5 + matrix[2][2] * s4 - matrix[2][3] * s3) * invDet;

        inv.matrix[1][0] = (-matrix[1][0] * c5 + matrix[1][2] * c2 - matrix[1][3] * c1) * invDet;
        inv.matrix[1][1] = (matrix[0][0] * c5 - matrix[0][2] * c2 + matrix[0][3] * c1) * invDet;
        inv.matrix[1][2] = (-matrix[3][0] * s5 + matrix[3][2] * s2 - matrix[3][3] * s1) * invDet;
        inv.matrix[1][3] = (matrix[2][0] * s5 - matrix[2][2] * s2 + matrix[2][3] * s1) * invDet;

        inv.matrix[2][0] = (matrix[1][0] * c4 - matrix[1][1] * c2 + matrix[1][3] * c0) * invDet;
        inv.matrix[2][1] = (-matrix[0][0] * c4 + matrix[0][1] * c2 - matrix[0][3] * c0) * invDet;
        inv.matrix[2][2] = (matrix[3][0] * s4 - matrix[3][1] * s2 + matrix[3][3] * s0) * invDet;
        inv.matrix[2][3] = (-matrix[2][0] * s4 + matrix[2][1] * s2 - matrix[2][3] * s0) * invDet;

        inv.matrix[3][0] = (-matrix[1][0] * c3 + matrix[1][1] * c1 - matrix[1][2] * c0) * invDet;
        inv.matrix[3][1] = (matrix[0][0] * c3 - matrix[0][1] * c1 + matrix[0][2] * c0) * invDet;
        inv.matrix[3][2] = (-matrix[3][0] * s3 + matrix[3][1] * s1 - matrix[3][2] * s0) * invDet;
        inv.matrix[3][3] = (matrix[2][0] * s3 - matrix[2][1] * s1 + matrix[2][2] * s0) * invDet;

        return inv;
    }

    Matrix4 Matrix4::identity() { return IDENTITY; }

    Matrix4 Matrix4::translate(const Vector3& translation) {
        Matrix4 result(1.0f);
        result.matrix[0][3] = translation.x;
        result.matrix[1][3] = translation.y;
        result.matrix[2][3] = translation.z;
        return result;
    }

    Matrix4 Matrix4::rotate(const Quaternion& rotation) {
        Matrix4 result(1.0f);
        float qx2 = rotation.x * rotation.x;
        float qy2 = rotation.y * rotation.y;
        float qz2 = rotation.z * rotation.z;

        result.matrix[0][0] = 1.0f - 2.0f * (qy2 + qz2);
        result.matrix[0][1] = 2.0f * (rotation.x * rotation.y - rotation.z * rotation.w);
        result.matrix[0][2] = 2.0f * (rotation.x * rotation.z + rotation.y * rotation.w);

        result.matrix[1][0] = 2.0f * (rotation.x * rotation.y + rotation.z * rotation.w);
        result.matrix[1][1] = 1.0f - 2.0f * (qx2 + qz2);
        result.matrix[1][2] = 2.0f * (rotation.y * rotation.z - rotation.x * rotation.w);

        result.matrix[2][0] = 2.0f * (rotation.x * rotation.z - rotation.y * rotation.w);
        result.matrix[2][1] = 2.0f * (rotation.y * rotation.z + rotation.x * rotation.w);
        result.matrix[2][2] = 1.0f - 2.0f * (qx2 + qy2);
        return result;
    }

    Matrix4 Matrix4::scale(const Vector3& factors) {
        Matrix4 result(1.0f);
        result.matrix[0][0] = factors.x;
        result.matrix[1][1] = factors.y;
        result.matrix[2][2] = factors.z;
        return result;
    }

    Matrix4 Matrix4::orthographic(float left, float right, float bottom, float top, float zNear, float zFar) {
        assert(right != left);
        assert(top != bottom);
        assert(zFar != zNear);

        Matrix4 result(1.0f);
        result.matrix[0][0] = 2.0f / (right - left);
        result.matrix[1][1] = -2.0f / (top - bottom); // Vulkan standard Y-down clip-space rule
        result.matrix[2][2] = 1.0f / (zFar - zNear);   // Vulkan standard [0, 1] forward depth rule
        result.matrix[0][3] = -(right + left) / (right - left);
        result.matrix[1][3] = -(top + bottom) / (top - bottom);
        result.matrix[2][3] = -zNear / (zFar - zNear);
        return result;
    }

    Matrix4 Matrix4::perspective(float fov, float aspect, float zNear, float zFar) {
        assert(aspect != 0.0f);
        assert(zFar != zNear);
        float tanHalfFovy = std::tan(fov * 0.5f);

        Matrix4 result;
        result.matrix[0][0] = 1.0f / (aspect * tanHalfFovy);
        result.matrix[1][1] = -1.0f / tanHalfFovy;   // Vulkan standard Y-down clip-space rule
        result.matrix[2][2] = zFar / (zFar - zNear);  // Vulkan standard [0, 1] forward depth rule
        result.matrix[2][3] = -(zFar * zNear) / (zFar - zNear);
        result.matrix[3][2] = 1.0f;
        return result;
    }

    Matrix4 Matrix4::look(const Vector3& eye, const Vector3& target, const Vector3& up) {
        Vector3 zaxis = (target - eye).normalized();
        Vector3 xaxis = zaxis.cross(up).normalized();
        Vector3 yaxis = xaxis.cross(zaxis);

        Matrix4 view(1.0f);
        view.matrix[0][0] = xaxis.x; view.matrix[0][1] = xaxis.y; view.matrix[0][2] = xaxis.z;
        view.matrix[1][0] = yaxis.x; view.matrix[1][1] = yaxis.y; view.matrix[1][2] = yaxis.z;
        view.matrix[2][0] = zaxis.x; view.matrix[2][1] = zaxis.y; view.matrix[2][2] = zaxis.z;

        view.matrix[0][3] = -xaxis.dot(eye);
        view.matrix[1][3] = -yaxis.dot(eye);
        view.matrix[2][3] = -zaxis.dot(eye);
        return view;
    }

    bool Matrix4::decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const {
        translation = Vector3(matrix[0][3], matrix[1][3], matrix[2][3]);

        scale.x = Vector3(matrix[0][0], matrix[1][0], matrix[2][0]).length();
        scale.y = Vector3(matrix[0][1], matrix[1][1], matrix[2][1]).length();
        scale.z = Vector3(matrix[0][2], matrix[1][2], matrix[2][2]).length();

        if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f) return false;

        Matrix4 pure = *this;
        pure.matrix[0][0] /= scale.x; pure.matrix[1][0] /= scale.x; pure.matrix[2][0] /= scale.x;
        pure.matrix[0][1] /= scale.y; pure.matrix[1][1] /= scale.y; pure.matrix[2][1] /= scale.y;
        pure.matrix[0][2] /= scale.z; pure.matrix[1][2] /= scale.z; pure.matrix[2][2] /= scale.z;

        float trace = pure.matrix[0][0] + pure.matrix[1][1] + pure.matrix[2][2];
        if (trace > 0.0f) {
            float root = std::sqrt(trace + 1.0f) * 2.0f;
            rotation.w = 0.25f * root;
            rotation.x = (pure.matrix[2][1] - pure.matrix[1][2]) / root;
            rotation.y = (pure.matrix[0][2] - pure.matrix[2][0]) / root;
            rotation.z = (pure.matrix[1][0] - pure.matrix[0][1]) / root;
        } else if ((pure.matrix[0][0] > pure.matrix[1][1]) && (pure.matrix[0][0] > pure.matrix[2][2])) {
            float root = std::sqrt(1.0f + pure.matrix[0][0] - pure.matrix[1][1] - pure.matrix[2][2]) * 2.0f;
            rotation.w = (pure.matrix[2][1] - pure.matrix[1][2]) / root;
            rotation.x = 0.25f * root;
            rotation.y = (pure.matrix[0][1] + pure.matrix[1][0]) / root;
            rotation.z = (pure.matrix[0][2] + pure.matrix[2][0]) / root;
        } else if (pure.matrix[1][1] > pure.matrix[2][2]) {
            float root = std::sqrt(1.0f + pure.matrix[1][1] - pure.matrix[0][0] - pure.matrix[2][2]) * 2.0f;
            rotation.w = (pure.matrix[0][2] - pure.matrix[2][0]) / root;
            rotation.x = (pure.matrix[0][1] + pure.matrix[1][0]) / root;
            rotation.y = 0.25f * root;
            rotation.z = (pure.matrix[1][2] + pure.matrix[2][1]) / root;
        } else {
            float root = std::sqrt(1.0f + pure.matrix[2][2] - pure.matrix[0][0] - pure.matrix[1][1]) * 2.0f;
            rotation.w = (pure.matrix[1][0] - pure.matrix[0][1]) / root;
            rotation.x = (pure.matrix[0][2] + pure.matrix[2][0]) / root;
            rotation.y = (pure.matrix[1][2] + pure.matrix[2][1]) / root;
            rotation.z = 0.25f * root;
        }
        return true;
    }
}