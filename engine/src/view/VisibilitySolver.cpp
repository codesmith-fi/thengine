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
    struct TempVertex {
        Vector2 position;
        float angle;
    };
    std::vector<TempVertex> tempVertices;
    tempVertices.reserve(uniqueAngles.size());

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
        tempVertices.push_back({ hit, angle });
    }

    // Sort initial tempVertices by angle
    std::sort(tempVertices.begin(), tempVertices.end(), [](const TempVertex& a, const TempVertex& b) {
        return a.angle < b.angle;
    });

    // F. Adaptive Subdivision Step
    const float MAX_EDGE_LENGTH = 40.0f; // threshold in pixels
    const int MAX_DEPTH = 3;
    bool subdivided = true;
    int depth = 0;

    while (subdivided && depth < MAX_DEPTH) {
        subdivided = false;
        std::vector<TempVertex> nextVertices;
        nextVertices.reserve(tempVertices.size() * 2);

        for (size_t i = 0; i < tempVertices.size(); ++i) {
            const auto& A = tempVertices[i];
            const auto& B = tempVertices[(i + 1) % tempVertices.size()];

            nextVertices.push_back(A);

            float dx = B.position.x - A.position.x;
            float dy = B.position.y - A.position.y;
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist > MAX_EDGE_LENGTH) {
                float angleA = A.angle;
                float angleB = B.angle;
                if (angleB < angleA) {
                    angleB += 2.0f * M_PI;
                }
                float midAngle = (angleA + angleB) * 0.5f;

                Vector2 direction(std::cos(midAngle), std::sin(midAngle));
                float closest_t = radius;

                for (const auto& segment : segments) {
                    Vector2 SegA = segment.start;
                    Vector2 SegB = segment.end;
                    Vector2 T = SegA - center;
                    Vector2 V = SegB - SegA;

                    float denominator = direction.x * V.y - direction.y * V.x;
                    if (std::abs(denominator) > 1e-6f) {
                        float t = (T.x * V.y - T.y * V.x) / denominator;
                        float u = (T.x * direction.y - T.y * direction.x) / denominator;
                        if (t >= 0.0f && t < closest_t && u >= 0.0f && u <= 1.0f) {
                            closest_t = t;
                        }
                    }
                }

                Vector2 midPos = center + direction * closest_t;
                float normMidAngle = std::fmod(midAngle, 2.0f * M_PI);
                if (normMidAngle < 0.0f) normMidAngle += 2.0f * M_PI;

                nextVertices.push_back({ midPos, normMidAngle });
                subdivided = true;
            }
        }
        tempVertices = std::move(nextVertices);
        depth++;
    }

    // Collect final sorted vertices
    std::vector<Vector2> finalVertices;
    finalVertices.reserve(tempVertices.size());
    for (const auto& tv : tempVertices) {
        finalVertices.push_back(tv.position);
    }

    // Sort strictly clockwise/counter-clockwise relative to center
    std::sort(finalVertices.begin(), finalVertices.end(), [&center](const Vector2& a, const Vector2& b) {
        return std::atan2(a.y - center.y, a.x - center.x) < std::atan2(b.y - center.y, b.x - center.x);
    });

    VisibilityPolygon polygon;
    polygon.vertices = std::move(finalVertices);
    return polygon;
}

} // namespace thengine
