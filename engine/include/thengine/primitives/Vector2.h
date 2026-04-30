#pragma once

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
};

} // namespace thengine
