#pragma once
#include "thengine/primitives/Vector2.h"
#include "thengine/primitives/Color.h"
#include "thengine/view/VisibilityTypes.h"

namespace thengine {

struct LightSource {
    Vector2 position;
    float baseRadius;
    float currentRadius;
    Color color;
    float flickerTime;
    float intensity; // 1.0f for normal, higher for blinding spells, lower for dim lights
    bool active;
    bool isStatic;
    int lightType = 0; // Ready for future magic types (0 = torch flicker, 1 = steady magic globe)
    bool hasCachedPolygon;
    VisibilityPolygon cachedPolygon;

    LightSource(const Vector2& pos, float r, const Color& col, float intens = 1.0f, bool isStat = false)
        : position(pos), baseRadius(r), currentRadius(r), color(col), flickerTime(0.0f),
          intensity(intens), active(true), isStatic(isStat), lightType(0),
          hasCachedPolygon(false), cachedPolygon() {}
};

} // namespace thengine
