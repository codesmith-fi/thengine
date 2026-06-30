#pragma once
#include "VisibilityTypes.h"
#include "IObstacleProvider.h"
#include "thengine/primitives/Vector2.h"

namespace thengine {

class VisibilitySolver {
public:
    static VisibilityPolygon calculateVisibility(
        const Vector2& center,
        float radius,
        const IObstacleProvider& obstacleProvider
    );
};

} // namespace thengine
