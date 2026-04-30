#include "thengine/Entity.h"

namespace thengine {

Entity::Entity() : m_position(0.0f, 0.0f), m_scale(1.0f, 1.0f), m_rotation(0.0f) {}

Entity::~Entity() = default;

void Entity::update(float /*dt*/) {}

} // namespace thengine
