#pragma once

#include "thengine/primitives/Vector2.h"

namespace thengine {

class SpriteBatch;

class Entity {
public:
    Entity();
    virtual ~Entity();

    virtual void update(float dt);
    virtual void render(SpriteBatch& batch) = 0;
    
    void move(const Vector2& velocity);

    void setPosition(const Vector2& position) { m_position = position; }
    [[nodiscard]] const Vector2& getPosition() const { return m_position; }

    void setScale(const Vector2& scale) { m_scale = scale; }
    [[nodiscard]] const Vector2& getScale() const { return m_scale; }

    void setRotation(float rotation) { m_rotation = rotation; }
    [[nodiscard]] float getRotation() const { return m_rotation; }
    void rotate(float angle) { m_rotation += angle; }

private:
    Vector2 m_position;
    Vector2 m_scale;
    float m_rotation;
};

} // namespace thengine
