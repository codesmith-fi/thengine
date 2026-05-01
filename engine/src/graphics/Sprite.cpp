#include "thengine/graphics/Sprite.h"
#include "thengine/graphics/SpriteBatch.h"

namespace thengine {

Sprite::Sprite() : Entity(), m_color(255, 255, 255, 255), m_origin(0.5f, 0.5f), m_depth(0.0f) {}

Sprite::~Sprite() = default;

void Sprite::render(SpriteBatch& batch) {
    if (m_texture) {
        batch.draw(m_texture, getPosition(), getScale(), getRotation(), m_origin, m_color, m_depth);
    }
}

} // namespace thengine
