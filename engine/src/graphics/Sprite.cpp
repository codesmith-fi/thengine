#include "thengine/graphics/Sprite.h"
#include "thengine/Renderer.h"

namespace thengine {

Sprite::Sprite() : Entity(), m_color(255, 255, 255, 255) {}

Sprite::~Sprite() = default;

void Sprite::render(Renderer& renderer) {
    if (m_texture) {
        renderer.drawTexture(m_texture, m_position, m_scale, m_color);
    }
}

} // namespace thengine
