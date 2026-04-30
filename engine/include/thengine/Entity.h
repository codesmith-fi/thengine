#pragma once

#include "thengine/primitives/Vector2.h"

namespace thengine {

class Renderer;

class Entity {
public:
    Entity();
    virtual ~Entity();

    virtual void update(float dt);
    virtual void render(Renderer& renderer) = 0;
    
    void move(const Vector2& velocity);

    Vector2 m_position;
    Vector2 m_scale;
    float m_rotation;
};

} // namespace thengine
