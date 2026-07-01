#pragma once

namespace thengine {

enum class BlendMode {
    Alpha,
    Additive,
    Multiplicative
};

struct PointLight {
    float position[2];
    float radius;
    float intensity;
    float color[4];
};

} // namespace thengine
