#pragma once
#include "thengine/primitives/Vector2.h"
#include "thengine/primitives/Color.h"
#include "thengine/view/VisibilityTypes.h"

namespace thengine {

struct LightSource {
    Vector2 position;
    float radius;
    Color color;
    float intensity;
    bool active;
    bool isStatic;
    bool hasCachedPolygon;
    VisibilityPolygon cachedPolygon;

    LightSource(const Vector2& pos, float r, const Color& col, float intens = 1.0f, bool isStat = false)
        : position(pos), radius(r), color(col), intensity(intens), active(true),
          isStatic(isStat), hasCachedPolygon(false), cachedPolygon() {}
};

} // namespace thengine
