#pragma once

#include "thengine/primitives/Vector2.h"
#include "thengine/math/Matrix4.h"
#include <algorithm>

namespace thengine {

struct Camera2D {
private:
    Vector2 m_position = { 0.0f, 0.0f };
    float m_rotation = 0.0f;
    float m_zoom = 1.0f;
    
    mutable bool m_isDirty = true;
    mutable Matrix4 m_cachedTransform;
    mutable float m_lastScreenWidth = 0.0f;
    mutable float m_lastScreenHeight = 0.0f;

public:
    void setPosition(const Vector2& pos) { m_position = pos; m_isDirty = true; }
    const Vector2& getPosition() const { return m_position; }
    
    void setRotation(float rot) { m_rotation = rot; m_isDirty = true; }
    float getRotation() const { return m_rotation; }
    
    void setZoom(float z) { m_zoom = z; m_isDirty = true; }
    float getZoom() const { return m_zoom; }

    Matrix4 getTransform(float screenWidth, float screenHeight) const {
        if (screenWidth != m_lastScreenWidth || screenHeight != m_lastScreenHeight) {
            m_isDirty = true;
            m_lastScreenWidth = screenWidth;
            m_lastScreenHeight = screenHeight;
        }

        if (m_isDirty) {
            // Translate world coordinates by negative camera position
            Matrix4 translation = Matrix4::createTranslation(-m_position.x, -m_position.y);
            
            // Scale/zoom and rotate
            Matrix4 scale = Matrix4::createScale(m_zoom, m_zoom);
            Matrix4 rotationMat = Matrix4::createRotationZ(m_rotation);
            
            // Offset center of screen viewport back
            Matrix4 viewportCenter = Matrix4::createTranslation(screenWidth * 0.5f, screenHeight * 0.5f);

            // Transform is applied right-to-left: translation -> scale -> rotation -> viewportCenter
            m_cachedTransform = viewportCenter * rotationMat * scale * translation;
            m_isDirty = false;
        }
        return m_cachedTransform;
    }

    [[nodiscard]] bool isVisible(const Vector2& worldPos,
                                 const Vector2& size,
                                 float rotationVal,
                                 const Vector2& origin,
                                 const Matrix4& viewMat,
                                 float screenWidth,
                                 float screenHeight) const noexcept {
        float w = size.x;
        float h = size.y;
        float ox = origin.x * w;
        float oy = origin.y * h;

        float c = std::cos(rotationVal);
        float s = std::sin(rotationVal);

        auto transformLocalToWorld = [&](float lx, float ly) noexcept {
            float rx = lx * c - ly * s;
            float ry = lx * s + ly * c;
            return Vector2(worldPos.x + rx, worldPos.y + ry);
        };

        Vector2 p0 = transformLocalToWorld(-ox, -oy);
        Vector2 p1 = transformLocalToWorld(w - ox, -oy);
        Vector2 p2 = transformLocalToWorld(-ox, h - oy);
        Vector2 p3 = transformLocalToWorld(w - ox, h - oy);

        // Transform the four corners into view space using the passed viewMat
        auto transformWorldToView = [&](const Vector2& pt) noexcept {
            float x = pt.x * viewMat.m[0] + pt.y * viewMat.m[4] + viewMat.m[12];
            float y = pt.x * viewMat.m[1] + pt.y * viewMat.m[5] + viewMat.m[13];
            return Vector2(x, y);
        };

        Vector2 v0 = transformWorldToView(p0);
        Vector2 v1 = transformWorldToView(p1);
        Vector2 v2 = transformWorldToView(p2);
        Vector2 v3 = transformWorldToView(p3);

        // Find the bounding box of the transformed corners in view space
        float minX = std::min({v0.x, v1.x, v2.x, v3.x});
        float maxX = std::max({v0.x, v1.x, v2.x, v3.x});
        float minY = std::min({v0.y, v1.y, v2.y, v3.y});
        float maxY = std::max({v0.y, v1.y, v2.y, v3.y});

        // Check if the view-space bounding box intersects the viewport bounds [0, screenWidth] x [0, screenHeight]
        return !(maxX < 0.0f || minX > screenWidth || maxY < 0.0f || minY > screenHeight);
    }
};

} // namespace thengine
