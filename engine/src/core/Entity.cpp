#include "thengine/Entity.h"
#include "thengine/graphics/SpriteBatch.h"

namespace thengine {

Entity::Entity() : m_position(0.0f, 0.0f), m_scale(1.0f, 1.0f), m_rotation(0.0f) {}

Entity::~Entity() = default;

void Entity::update(float /*dt*/) {}

void Entity::move(const Vector2& velocity) {
    m_position += velocity;
}

} // namespace thengine
