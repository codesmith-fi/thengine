#pragma once

#include "thengine/primitives/Vector2.h"
#include "thengine/math/Matrix4.h"

namespace thengine {

struct Camera2D {
    Vector2 position = { 0.0f, 0.0f };
    float rotation = 0.0f;
    float zoom = 1.0f;

    Matrix4 getTransform(float screenWidth, float screenHeight) const {
        // Translate world coordinates by negative camera position
        Matrix4 translation = Matrix4::createTranslation(-position.x, -position.y);
        
        // Scale/zoom and rotate
        Matrix4 scale = Matrix4::createScale(zoom, zoom);
        Matrix4 rotationMat = Matrix4::createRotationZ(rotation);
        
        // Offset center of screen viewport back
        Matrix4 viewportCenter = Matrix4::createTranslation(screenWidth * 0.5f, screenHeight * 0.5f);

        // Transform is applied right-to-left: translation -> scale -> rotation -> viewportCenter
        return viewportCenter * rotationMat * scale * translation;
    }
};

} // namespace thengine
