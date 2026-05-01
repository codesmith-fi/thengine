#pragma once

#include "thengine/Entity.h"
#include "thengine/primitives/Color.h"
#include <memory>

namespace thengine {

class Texture;

class Sprite : public Entity {
public:
    Sprite();
    ~Sprite() override;

    void render(Renderer& renderer) override;

    void setRotation(float angle) { m_rotation = angle; }
    void rotate(float angle) { m_rotation += angle; }
    void setOrigin(const Vector2& origin) { m_origin = origin; }

    Color m_color;
    std::shared_ptr<Texture> m_texture;
    float m_rotation;
    Vector2 m_origin;
};

} // namespace thengine
