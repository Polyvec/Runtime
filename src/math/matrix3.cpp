#include "../include/math/matrix3.hpp"
#include <cassert>
#include <numbers>

namespace voxyl::math {

    const Matrix3 Matrix3::IDENTITY = Matrix3(1.0f);

    float Matrix3::operator()(int row, int column) const {
        assert(row >= 0 && row < 3);
        assert(column >= 0 && column < 3);
        return this->matrix[row][column];
    }

    float &Matrix3::operator()(int row, int column) {
        assert(row >= 0 && row < 3);
        assert(column >= 0 && column < 3);
        return this->matrix[row][column];
    }

    Matrix3 Matrix3::operator+(const Matrix3 &other) const {
        Matrix3 result;
        for (int row = 0; row < 3; row++)
            for (int column = 0; column < 3; column++)
                result.matrix[row][column] = this->matrix[row][column] + other.matrix[row][column];
        return result;
    }

    Matrix3 Matrix3::operator-(const Matrix3 &other) const {
        Matrix3 result;
        for (int row = 0; row < 3; row++)
            for (int column = 0; column < 3; column++)
                result.matrix[row][column] = this->matrix[row][column] - other.matrix[row][column];
        return result;
    }

    Matrix3 Matrix3::operator*(const Matrix3 &other) const {
        Matrix3 result;
        for (int row = 0; row < 3; row++) {
            for (int column = 0; column < 3; column++) {
                result.matrix[row][column] = 0.0f;
                for (int index = 0; index < 3; index++) {
                    result.matrix[row][column] += this->matrix[row][index] * other.matrix[index][column];
                }
            }
        }
        return result;
    }

    Matrix3 Matrix3::operator*(float scalar) const {
        Matrix3 result;
        for (int row = 0; row < 3; row++)
            for (int column = 0; column < 3; column++)
                result.matrix[row][column] = this->matrix[row][column] * scalar;
        return result;
    }

    Vector2 Matrix3::operator*(const Vector2 &vector) const {
        return Vector2(
            matrix[0][0] * vector.x + matrix[0][1] * vector.y + matrix[0][2],
            matrix[1][0] * vector.x + matrix[1][1] * vector.y + matrix[1][2]
        );
    }

    Matrix3 &Matrix3::operator+=(const Matrix3 &other) {
        *this = *this + other; return *this;
    }
    Matrix3 &Matrix3::operator-=(const Matrix3 &other) {
        *this = *this - other; return *this;
    }
    Matrix3 &Matrix3::operator*=(const Matrix3 &other) {
        *this = *this * other; return *this;
    }
    Matrix3 &Matrix3::operator*=(float scalar) {
        *this = *this * scalar; return *this;
    }

    bool Matrix3::operator==(const Matrix3 &other) const {
        for (int row = 0; row < 3; row++)
            for (int column = 0; column < 3; column++)
                if (matrix[row][column] != other.matrix[row][column]) return false;
        return true;
    }

    bool Matrix3::operator!=(const Matrix3 &other) const {
        return !(*this == other);
    }

    Matrix3 Matrix3::transposed() const {
        Matrix3 result;
        for (int row = 0; row < 3; row++)
            for (int column = 0; column < 3; column++)
                result.matrix[column][row] = matrix[row][column];
        return result;
    }

    Scalar Matrix3::determinant() const {
        float det = matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
                    matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
                    matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
        return Scalar(det);
    }

    Matrix3 Matrix3::inverted() const {
        float det = (float)determinant();
        assert(det != 0.0f);
        float invDet = 1.0f / det;
        Matrix3 result;
        result.matrix[0][0] = (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) * invDet;
        result.matrix[0][1] = (matrix[0][2] * matrix[2][1] - matrix[0][1] * matrix[2][2]) * invDet;
        result.matrix[0][2] = (matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1]) * invDet;
        result.matrix[1][0] = (matrix[1][2] * matrix[2][0] - matrix[1][0] * matrix[2][2]) * invDet;
        result.matrix[1][1] = (matrix[0][0] * matrix[2][2] - matrix[0][2] * matrix[2][0]) * invDet;
        result.matrix[1][2] = (matrix[0][2] * matrix[1][0] - matrix[0][0] * matrix[1][2]) * invDet;
        result.matrix[2][0] = (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]) * invDet;
        result.matrix[2][1] = (matrix[0][1] * matrix[2][0] - matrix[0][0] * matrix[2][1]) * invDet;
        result.matrix[2][2] = (matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]) * invDet;
        return result;
    }

    bool Matrix3::approximately(const Matrix3 &other, float epsilon) const {
        for (int row = 0; row < 3; row++)
            for (int column = 0; column < 3; column++)
                if (std::fabs(matrix[row][column] - other.matrix[row][column]) > epsilon) return false;
        return true;
    }

    Vector2 Matrix3::translation() const { return Vector2(matrix[0][2], matrix[1][2]); }
    Scalar Matrix3::rotation() const { return Scalar(std::atan2(matrix[1][0], matrix[0][0])); }
    Vector2 Matrix3::scale() const {
        return Vector2(std::sqrt(matrix[0][0]*matrix[0][0] + matrix[1][0]*matrix[1][0]),
                       std::sqrt(matrix[0][1]*matrix[0][1] + matrix[1][1]*matrix[1][1]));
    }

    Vector2 Matrix3::superior() const { return Vector2(matrix[0][0], matrix[0][1]); }
    Vector2 Matrix3::inferior() const { return Vector2(matrix[2][0], matrix[2][1]); }
    Vector2 Matrix3::dexter() const { return Vector2(matrix[0][0], matrix[1][0]); }
    Vector2 Matrix3::sinister() const { return Vector2(matrix[0][2], matrix[1][2]); }

    bool Matrix3::decompose(Vector2 &translation, float &rotation, Vector2 &scale) const {
        translation = this->translation();
        scale = this->scale();
        if (scale.x == 0.0f || scale.y == 0.0f) return false;
        rotation = std::atan2(matrix[1][0] / scale.x, matrix[0][0] / scale.x);
        return true;
    }

    Matrix3 Matrix3::identity() { return IDENTITY; }
    Matrix3 Matrix3::translate(const Vector2 &translation) {
        Matrix3 result(1.0f);
        result.matrix[0][2] = translation.x;
        result.matrix[1][2] = translation.y;
        return result;
    }
    Matrix3 Matrix3::rotate(float angle) {
        float rad = angle * (std::numbers::pi_v<float> / 180.0f);
        float cosine = std::cos(rad);
        float sine = std::sin(rad);
        Matrix3 result(1.0f);
        result.matrix[0][0] = cosine; result.matrix[0][1] = -sine;
        result.matrix[1][0] = sine;   result.matrix[1][1] = cosine;
        return result;
    }
    Matrix3 Matrix3::scale(const Vector2 &scale) {
        Matrix3 result(1.0f);
        result.matrix[0][0] = scale.x;
        result.matrix[1][1] = scale.y;
        return result;
    }
    Matrix3 Matrix3::compose(const Vector2 &translation, float rotation, const Vector2 &scale) {
        return translate(translation) * rotate(rotation) * Matrix3::scale(scale);
    }
    Matrix3 Matrix3::orthographic(float left, float right, float bottom, float top) {
        assert(right != left);
        assert(top != bottom);
        Matrix3 result(1.0f);
        result.matrix[0][0] = 2.0f / (right - left);
        result.matrix[1][1] = 2.0f / (top - bottom);
        result.matrix[0][2] = -(right + left) / (right - left);
        result.matrix[1][2] = -(top + bottom) / (top - bottom);
        return result;
    }

    std::ostream &operator<<(std::ostream &os, const Matrix3 &other) {
        return os << "[[" << other.matrix[0][0] << ", " << other.matrix[0][1] << ", " << other.matrix[0][2] << "], "
                  << "[" << other.matrix[1][0] << ", " << other.matrix[1][1] << ", " << other.matrix[1][2] << "], "
                  << "[" << other.matrix[2][0] << ", " << other.matrix[2][1] << ", " << other.matrix[2][2] << "]]";
    }
}