#include "thengine/graphics/Sprite.h"
#include "thengine/Renderer.h"

namespace thengine {

Sprite::Sprite() : Entity(), m_color(255, 255, 255, 255), m_origin(0.5f, 0.5f), m_depth(0.0f) {}

Sprite::~Sprite() = default;

void Sprite::render(Renderer& renderer) {
    if (m_texture) {
        renderer.drawTexture(m_texture, getPosition(), getScale(), getRotation(), m_origin, m_color);
    }
}

} // namespace thengine
