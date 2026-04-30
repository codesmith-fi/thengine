#pragma once

#include <cmath>

namespace thengine {

struct Vector2 {
    float x;
    float y;

    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x_, float y_) : x(x_), y(y_) {}

    Vector2 operator+(const Vector2& rhs) const noexcept { return Vector2(x + rhs.x, y + rhs.y); }
    Vector2 operator-(const Vector2& rhs) const noexcept { return Vector2(x - rhs.x, y - rhs.y); }
    Vector2 operator*(float scalar) const noexcept { return Vector2(x * scalar, y * scalar); }
    Vector2 operator/(float scalar) const noexcept { return Vector2(x / scalar, y / scalar); }

    Vector2& operator+=(const Vector2& rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }
    Vector2& operator-=(const Vector2& rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }
    Vector2& operator*=(float scalar) noexcept { x *= scalar; y *= scalar; return *this; }
    Vector2& operator/=(float scalar) noexcept { x /= scalar; y /= scalar; return *this; }

    [[nodiscard]] float length() const noexcept {
        return std::sqrt(x * x + y * y);
    }

    [[nodiscard]] Vector2 normalized() const noexcept {
        float len = length();
        if (len > 0.0001f) {
            return {x / len, y / len};
        }
        return {0.0f, 0.0f};
    }
};

} // namespace thengine
