#include "thengine/graphics/SpriteBatch.h"
#include "thengine/Renderer.h"
#include "thengine/graphics/Texture.h"
#include "thengine/graphics/SpriteEffect.h"
#include <algorithm>
#include <cmath>

namespace thengine {

SpriteBatch::SpriteBatch(Renderer& renderer) : m_renderer(renderer), m_isBegun(false) {}

SpriteBatch::~SpriteBatch() {}

void SpriteBatch::begin(std::shared_ptr<SpriteEffect> effect) {
    if (m_isBegun) return;
    
    m_currentEffect = effect;
    m_sprites.clear();
    m_isBegun = true;
}

void SpriteBatch::draw(std::shared_ptr<Texture> texture, 
                       const Vector2& position, 
                       const Vector2& scale, 
                       float rotation, 
                       const Vector2& origin, 
                       const Color& color, 
                       float depth) {
    if (!m_isBegun || !texture) return;
    
    m_sprites.push_back({texture, position, scale, rotation, origin, color, depth});
}

void SpriteBatch::end() {
    if (!m_isBegun) return;
    
    if (!m_sprites.empty()) {
        std::sort(m_sprites.begin(), m_sprites.end(), [](const SpriteDrawCommand& a, const SpriteDrawCommand& b) {
            if (a.depth != b.depth) {
                return a.depth < b.depth;
            }
            return a.texture.get() < b.texture.get();
        });
        
        flush();
    }
    
    m_isBegun = false;
    m_sprites.clear();
    m_currentEffect = nullptr;
}

void SpriteBatch::flush() {
    if (m_sprites.empty()) return;
    
    std::shared_ptr<Texture> currentTexture = m_sprites[0].texture;
    m_vertexBuffer.clear();
    
    for (size_t i = 0; i <= m_sprites.size(); ++i) {
        if (i == m_sprites.size() || m_sprites[i].texture != currentTexture) {
            // Flush current batch
            if (!m_vertexBuffer.empty()) {
                m_renderer.drawBatched(currentTexture, m_vertexBuffer.data(), m_vertexBuffer.size());
                m_vertexBuffer.clear();
            }
            
            if (i < m_sprites.size()) {
                currentTexture = m_sprites[i].texture;
            }
        }
        
        if (i < m_sprites.size()) {
            const auto& sprite = m_sprites[i];
            
            float w = static_cast<float>(sprite.texture->getWidth()) * sprite.scale.x;
            float h = static_cast<float>(sprite.texture->getHeight()) * sprite.scale.y;
            
            float ox = sprite.origin.x * w;
            float oy = sprite.origin.y * h;
            
            float c = std::cos(sprite.rotation);
            float s = std::sin(sprite.rotation);
            
            auto transform = [&](float lx, float ly) {
                float rx = lx * c - ly * s;
                float ry = lx * s + ly * c;
                return Vector2(sprite.position.x + rx, sprite.position.y + ry);
            };
            
            Vector2 p0 = transform(-ox, -oy);
            Vector2 p1 = transform(w - ox, -oy);
            Vector2 p2 = transform(-ox, h - oy);
            Vector2 p3 = transform(w - ox, h - oy);
            
            float r = sprite.color.r / 255.0f;
            float g = sprite.color.g / 255.0f;
            float b = sprite.color.b / 255.0f;
            float a = sprite.color.a / 255.0f;
            
            // Push 6 vertices for the quad (TRIANGLELIST)
            m_vertexBuffer.push_back({p0.x, p0.y, 0.0f, 0.0f, r, g, b, a}); // Top-left
            m_vertexBuffer.push_back({p1.x, p1.y, 1.0f, 0.0f, r, g, b, a}); // Top-right
            m_vertexBuffer.push_back({p2.x, p2.y, 0.0f, 1.0f, r, g, b, a}); // Bottom-left

            m_vertexBuffer.push_back({p2.x, p2.y, 0.0f, 1.0f, r, g, b, a}); // Bottom-left
            m_vertexBuffer.push_back({p1.x, p1.y, 1.0f, 0.0f, r, g, b, a}); // Top-right
            m_vertexBuffer.push_back({p3.x, p3.y, 1.0f, 1.0f, r, g, b, a}); // Bottom-right
        }
    }
}

} // namespace thengine
