#pragma once

namespace thengine {

struct Rectangle {
    float x;
    float y;
    float width;
    float height;

    Rectangle() : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}
    Rectangle(float x, float y, float width, float height)
        : x(x), y(y), width(width), height(height) {}
};

} // namespace thengine
