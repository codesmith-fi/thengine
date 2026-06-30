#pragma once
#include "VisibilityTypes.h"
#include "thengine/primitives/Vector2.h"
#include <vector>

namespace thengine {

class IObstacleProvider {
public:
    virtual ~IObstacleProvider() = default;
    // Retrieves all obstacle line segments within a given radial bounds
    virtual std::vector<LineSegment> getObstacleEdges(const Vector2& center, float radius) const = 0;
};

} // namespace thengine
