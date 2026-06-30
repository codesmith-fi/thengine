#pragma once
#include "thengine/primitives/Vector2.h"
#include <vector>

namespace thengine {

struct LineSegment {
    Vector2 start;
    Vector2 end;
};

struct VisibilityPolygon {
    std::vector<Vector2> vertices;
};

} // namespace thengine
