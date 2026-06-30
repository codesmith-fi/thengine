#include "thengine/view/VisibilitySolver.h"
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace thengine {

VisibilityPolygon VisibilitySolver::calculateVisibility(
    const Vector2& center,
    float radius,
    const IObstacleProvider& obstacleProvider
) {
    // A. Query obstacles
    std::vector<LineSegment> segments = obstacleProvider.getObstacleEdges(center, radius);

    // B. Extract unique endpoints and generate angles
    std::vector<float> angles;

    // Add 16 default angles to ensure circular bounds approximation
    for (int i = 0; i < 16; ++i) {
        float angle = static_cast<float>(i) * (2.0f * M_PI / 16.0f);
        angles.push_back(angle);
    }

    for (const auto& segment : segments) {
        Vector2 points[2] = { segment.start, segment.end };
        for (const auto& p : points) {
            float angle = std::atan2(p.y - center.y, p.x - center.x);
            angles.push_back(angle);
            angles.push_back(angle + 0.0001f);
            angles.push_back(angle - 0.0001f);
        }
    }

    // C. Deduplicate and normalize unique angles
    std::vector<float> normalizedAngles;
    normalizedAngles.reserve(angles.size());
    for (float angle : angles) {
        float norm = std::fmod(angle, 2.0f * M_PI);
        if (norm < 0.0f) norm += 2.0f * M_PI;
        normalizedAngles.push_back(norm);
    }
    std::sort(normalizedAngles.begin(), normalizedAngles.end());

    std::vector<float> uniqueAngles;
    uniqueAngles.reserve(normalizedAngles.size());
    for (float angle : normalizedAngles) {
        if (uniqueAngles.empty() || std::abs(angle - uniqueAngles.back()) > 1e-5f) {
            if (uniqueAngles.empty() || std::abs(angle - uniqueAngles.front() - 2.0f * M_PI) > 1e-5f) {
                uniqueAngles.push_back(angle);
            }
        }
    }

    // D. Cast rays
    std::vector<Vector2> hitPoints;
    hitPoints.reserve(uniqueAngles.size());

    for (float angle : uniqueAngles) {
        Vector2 direction(std::cos(angle), std::sin(angle));

        float closest_t = radius;

        // E. Find closest intersection
        for (const auto& segment : segments) {
            Vector2 A = segment.start;
            Vector2 B = segment.end;
            Vector2 T = A - center;
            Vector2 V = B - A;

            float denominator = direction.x * V.y - direction.y * V.x;
            if (std::abs(denominator) > 1e-6f) {
                float t = (T.x * V.y - T.y * V.x) / denominator;
                float u = (T.x * direction.y - T.y * direction.x) / denominator;
                if (t >= 0.0f && t < closest_t && u >= 0.0f && u <= 1.0f) {
                    closest_t = t;
                }
            }
        }

        Vector2 hit = center + direction * closest_t;
        hitPoints.push_back(hit);
    }

    // F. Sort hitpoints strictly by their angle relative to the center
    std::sort(hitPoints.begin(), hitPoints.end(), [&center](const Vector2& a, const Vector2& b) {
        return std::atan2(a.y - center.y, a.x - center.x) < std::atan2(b.y - center.y, b.x - center.x);
    });

    VisibilityPolygon polygon;
    polygon.vertices = std::move(hitPoints);
    return polygon;
}

} // namespace thengine
