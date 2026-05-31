#include "../include/core/types/matrix4.hpp"
#include <cassert>

namespace core::math {

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
        return {
            this->matrix[0][0] * vector.x + this->matrix[0][1] * vector.y + this->matrix[0][2] * vector.z + this->matrix[0][3] * vector.w,
            this->matrix[1][0] * vector.x + this->matrix[1][1] * vector.y + this->matrix[1][2] * vector.z + this->matrix[1][3] * vector.w,
            this->matrix[2][0] * vector.x + this->matrix[2][1] * vector.y + this->matrix[2][2] * vector.z + this->matrix[2][3] * vector.w,
            this->matrix[3][0] * vector.x + this->matrix[3][1] * vector.y + this->matrix[3][2] * vector.z + this->matrix[3][3] * vector.w
        };
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
        float first[6] = {
            matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0],
            matrix[0][0] * matrix[1][2] - matrix[0][2] * matrix[1][0],
            matrix[0][0] * matrix[1][3] - matrix[0][3] * matrix[1][0],
            matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1],
            matrix[0][1] * matrix[1][3] - matrix[0][3] * matrix[1][1],
            matrix[0][2] * matrix[1][3] - matrix[0][3] * matrix[1][2]
        };

        float second[6] = {
            matrix[2][0] * matrix[3][1] - matrix[2][1] * matrix[3][0],
            matrix[2][0] * matrix[3][2] - matrix[2][2] * matrix[3][0],
            matrix[2][0] * matrix[3][3] - matrix[2][3] * matrix[3][0],
            matrix[2][1] * matrix[3][2] - matrix[2][2] * matrix[3][1],
            matrix[2][1] * matrix[3][3] - matrix[2][3] * matrix[3][1],
            matrix[2][2] * matrix[3][3] - matrix[2][3] * matrix[3][2]
        };

        float determinant = first[0] * second[5] - first[1] * second[4] + first[2] * second[3] + first[3] * second[2] - first[4] * second[1] + first[5] * second[0];
        assert(determinant != 0.0f);
        float inverse = 1.0f / determinant;

        Matrix4 result;
        result.matrix[0][0] = (matrix[1][1] * second[5] - matrix[1][2] * second[4] + matrix[1][3] * second[3]) * inverse;
        result.matrix[0][1] = (-matrix[0][1] * second[5] + matrix[0][2] * second[4] - matrix[0][3] * second[3]) * inverse;
        result.matrix[0][2] = (matrix[3][1] * first[5] - matrix[3][2] * first[4] + matrix[3][3] * first[3]) * inverse;
        result.matrix[0][3] = (-matrix[2][1] * first[5] + matrix[2][2] * first[4] - matrix[2][3] * first[3]) * inverse;

        result.matrix[1][0] = (-matrix[1][0] * second[5] + matrix[1][2] * second[2] - matrix[1][3] * second[1]) * inverse;
        result.matrix[1][1] = (matrix[0][0] * second[5] - matrix[0][2] * second[2] + matrix[0][3] * second[1]) * inverse;
        result.matrix[1][2] = (-matrix[3][0] * first[5] + matrix[3][2] * first[2] - matrix[3][3] * first[1]) * inverse;
        result.matrix[1][3] = (matrix[2][0] * first[5] - matrix[2][2] * first[2] + matrix[2][3] * first[1]) * inverse;

        result.matrix[2][0] = (matrix[1][0] * second[4] - matrix[1][1] * second[2] + matrix[1][3] * second[0]) * inverse;
        result.matrix[2][1] = (-matrix[0][0] * second[4] + matrix[0][1] * second[2] - matrix[0][3] * second[0]) * inverse;
        result.matrix[2][2] = (matrix[3][0] * first[4] - matrix[3][1] * first[2] + matrix[3][3] * first[0]) * inverse;
        result.matrix[2][3] = (-matrix[2][0] * first[4] + matrix[2][1] * first[2] - matrix[2][3] * first[0]) * inverse;

        result.matrix[3][0] = (-matrix[1][0] * second[3] + matrix[1][1] * second[1] - matrix[1][2] * second[0]) * inverse;
        result.matrix[3][1] = (matrix[0][0] * second[3] - matrix[0][1] * second[1] + matrix[0][2] * second[0]) * inverse;
        result.matrix[3][2] = (-matrix[3][0] * first[3] + matrix[3][1] * first[1] - matrix[3][2] * first[0]) * inverse;
        result.matrix[3][3] = (matrix[2][0] * first[3] - matrix[2][1] * first[1] + matrix[2][2] * first[0]) * inverse;

        return result;
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
        result.matrix[0][0] = 1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z);
        result.matrix[0][1] = 2.0f * (rotation.x * rotation.y - rotation.z * rotation.w);
        result.matrix[0][2] = 2.0f * (rotation.x * rotation.z + rotation.y * rotation.w);

        result.matrix[1][0] = 2.0f * (rotation.x * rotation.y + rotation.z * rotation.w);
        result.matrix[1][1] = 1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z);
        result.matrix[1][2] = 2.0f * (rotation.y * rotation.z - rotation.x * rotation.w);

        result.matrix[2][0] = 2.0f * (rotation.x * rotation.z - rotation.y * rotation.w);
        result.matrix[2][1] = 2.0f * (rotation.y * rotation.z + rotation.x * rotation.w);
        result.matrix[2][2] = 1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y);
        return result;
    }

    Matrix4 Matrix4::scale(const Vector3& factors) {
        Matrix4 result(1.0f);
        result.matrix[0][0] = factors.x;
        result.matrix[1][1] = factors.y;
        result.matrix[2][2] = factors.z;
        return result;
    }

    Matrix4 Matrix4::orthographic(float left, float right, float bottom, float top, float near, float far) {
        assert(right != left);
        assert(top != bottom);
        assert(far != near);

        Matrix4 result(1.0f);
        result.matrix[0][0] = 2.0f / (right - left);
        result.matrix[1][1] = -2.0f / (top - bottom);
        result.matrix[2][2] = 1.0f / (far - near);
        result.matrix[0][3] = -(right + left) / (right - left);
        result.matrix[1][3] = -(top + bottom) / (top - bottom);
        result.matrix[2][3] = -near / (far - near);
        return result;
    }

    Matrix4 Matrix4::perspective(float fov, float aspect, float near, float far) {
        assert(aspect != 0.0f);
        assert(far != near);
        float tangent = std::tan(fov * 0.5f);

        Matrix4 result;
        result.matrix[0][0] = 1.0f / (aspect * tangent);
        result.matrix[1][1] = -1.0f / tangent;
        result.matrix[2][2] = far / (far - near);
        result.matrix[2][3] = -(far * near) / (far - near);
        result.matrix[3][2] = 1.0f;
        return result;
    }

    Matrix4 Matrix4::look(const Vector3& eye, const Vector3& target, const Vector3& up) {
        Vector3 forward = (target - eye).normalized();
        Vector3 right = forward.cross(up).normalized();
        Vector3 upward = right.cross(forward);

        Matrix4 result(1.0f);
        result.matrix[0][0] = right.x;   result.matrix[0][1] = right.y;   result.matrix[0][2] = right.z;
        result.matrix[1][0] = upward.x;  result.matrix[1][1] = upward.y;  result.matrix[1][2] = upward.z;
        result.matrix[2][0] = forward.x; result.matrix[2][1] = forward.y; result.matrix[2][2] = forward.z;

        result.matrix[0][3] = -right.dot(eye);
        result.matrix[1][3] = -upward.dot(eye);
        result.matrix[2][3] = -forward.dot(eye);
        return result;
    }

    bool Matrix4::decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const {
        translation = {matrix[0][3], matrix[1][3], matrix[2][3]};

        scale.x = std::sqrt(matrix[0][0]*matrix[0][0] + matrix[1][0]*matrix[1][0] + matrix[2][0]*matrix[2][0]);
        scale.y = std::sqrt(matrix[0][1]*matrix[0][1] + matrix[1][1]*matrix[1][1] + matrix[2][1]*matrix[2][1]);
        scale.z = std::sqrt(matrix[0][2]*matrix[0][2] + matrix[1][2]*matrix[1][2] + matrix[2][2]*matrix[2][2]);

        if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f) return false;

        Matrix4 normalized = *this;
        normalized.matrix[0][0] /= scale.x; normalized.matrix[1][0] /= scale.x; normalized.matrix[2][0] /= scale.x;
        normalized.matrix[0][1] /= scale.y; normalized.matrix[1][1] /= scale.y; normalized.matrix[2][1] /= scale.y;
        normalized.matrix[0][2] /= scale.z; normalized.matrix[1][2] /= scale.z; normalized.matrix[2][2] /= scale.z;

        float trace = normalized.matrix[0][0] + normalized.matrix[1][1] + normalized.matrix[2][2];
        if (trace > 0.0f) {
            float root = std::sqrt(trace + 1.0f) * 2.0f;
            rotation.w = 0.25f * root;
            rotation.x = (normalized.matrix[2][1] - normalized.matrix[1][2]) / root;
            rotation.y = (normalized.matrix[0][2] - normalized.matrix[2][0]) / root;
            rotation.z = (normalized.matrix[1][0] - normalized.matrix[0][1]) / root;
        } else if ((normalized.matrix[0][0] > normalized.matrix[1][1]) && (normalized.matrix[0][0] > normalized.matrix[2][2])) {
            float root = std::sqrt(1.0f + normalized.matrix[0][0] - normalized.matrix[1][1] - normalized.matrix[2][2]) * 2.0f;
            rotation.w = (normalized.matrix[2][1] - normalized.matrix[1][2]) / root;
            rotation.x = 0.25f * root;
            rotation.y = (normalized.matrix[0][1] + normalized.matrix[1][0]) / root;
            rotation.z = (normalized.matrix[0][2] + normalized.matrix[2][0]) / root;
        } else if (normalized.matrix[1][1] > normalized.matrix[2][2]) {
            float root = std::sqrt(1.0f + normalized.matrix[1][1] - normalized.matrix[0][0] - normalized.matrix[2][2]) * 2.0f;
            rotation.w = (normalized.matrix[0][2] - normalized.matrix[2][0]) / root;
            rotation.x = (normalized.matrix[0][1] + normalized.matrix[1][0]) / root;
            rotation.y = 0.25f * root;
            rotation.z = (normalized.matrix[1][2] + normalized.matrix[2][1]) / root;
        } else {
            float root = std::sqrt(1.0f + normalized.matrix[2][2] - normalized.matrix[0][0] - normalized.matrix[1][1]) * 2.0f;
            rotation.w = (normalized.matrix[1][0] - normalized.matrix[0][1]) / root;
            rotation.x = (normalized.matrix[0][2] + normalized.matrix[2][0]) / root;
            rotation.y = (normalized.matrix[1][2] + normalized.matrix[2][1]) / root;
            rotation.z = 0.25f * root;
        }
        return true;
    }
}