#pragma once

namespace thengine::math {

constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 6.28318530717958647692f;

constexpr float degToRad(float degrees) noexcept {
    return degrees * (PI / 180.0f);
}

} // namespace thengine::math
