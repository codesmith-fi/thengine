#pragma once

#include <cmath>
#include <cstring>

namespace thengine {

struct Matrix4 {
    float m[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    static Matrix4 identity() {
        return Matrix4();
    }

    static Matrix4 createTranslation(float x, float y, float z = 0.0f) {
        Matrix4 result;
        result.m[12] = x;
        result.m[13] = y;
        result.m[14] = z;
        return result;
    }

    static Matrix4 createRotationZ(float radians) {
        Matrix4 result;
        float c = std::cos(radians);
        float s = std::sin(radians);
        result.m[0] = c;  result.m[1] = s;
        result.m[4] = -s; result.m[5] = c;
        return result;
    }

    static Matrix4 createScale(float x, float y, float z = 1.0f) {
        Matrix4 result;
        result.m[0] = x;
        result.m[5] = y;
        result.m[10] = z;
        return result;
    }

    static Matrix4 createOrthographic(float width, float height) {
        Matrix4 result;
        result.m[0] = 2.0f / width;
        result.m[5] = -2.0f / height;
        result.m[10] = 1.0f;
        result.m[12] = -1.0f;
        result.m[13] = 1.0f;
        result.m[15] = 1.0f;
        return result;
    }

    Matrix4 operator*(const Matrix4& other) const {
        Matrix4 result;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                float sum = 0.0f;
                for (int i = 0; i < 4; ++i) {
                    sum += m[row + i * 4] * other.m[i + col * 4];
                }
                result.m[row + col * 4] = sum;
            }
        }
        return result;
    }
};

} // namespace thengine
