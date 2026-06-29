#include "thengine/graphics/SpriteBatch.h"
#include "thengine/Renderer.h"
#include "thengine/graphics/Texture.h"
#include "thengine/graphics/SpriteEffect.h"
#include "thengine/graphics/SpriteFont.h"
#include <algorithm>
#include <cmath>

namespace {

// Helper function to decode UTF-8 bytes to UTF-32 codepoints
inline uint32_t decodeNextUtf8(std::string_view::const_iterator& it, std::string_view::const_iterator end) {
    if (it == end) return 0;
    uint8_t cp = static_cast<uint8_t>(*it);
    ++it;

    if (cp < 0x80) {
        return cp;
    } else if ((cp & 0xE0) == 0xC0) {
        if (it == end) return 0;
        uint32_t val = (cp & 0x1F) << 6;
        val |= (static_cast<uint8_t>(*it) & 0x3F);
        ++it;
        return val;
    } else if ((cp & 0xF0) == 0xE0) {
        if (it == end) return 0;
        uint32_t val = (cp & 0x0F) << 12;
        val |= (static_cast<uint8_t>(*it) & 0x3F) << 6;
        ++it;
        if (it == end) return 0;
        val |= (static_cast<uint8_t>(*it) & 0x3F);
        ++it;
        return val;
    } else if ((cp & 0xF8) == 0xF0) {
        if (it == end) return 0;
        uint32_t val = (cp & 0x07) << 18;
        val |= (static_cast<uint8_t>(*it) & 0x3F) << 12;
        ++it;
        if (it == end) return 0;
        val |= (static_cast<uint8_t>(*it) & 0x3F) << 6;
        ++it;
        if (it == end) return 0;
        val |= (static_cast<uint8_t>(*it) & 0x3F);
        ++it;
        return val;
    }
    return '?'; // Fallback for invalid UTF-8
}

} // namespace

namespace thengine {

SpriteBatch::SpriteBatch(Renderer& renderer) : m_renderer(renderer), m_isBegun(false) {}

SpriteBatch::~SpriteBatch() {}

void SpriteBatch::begin(std::shared_ptr<SpriteEffect> effect, const Matrix4& transformMatrix) {
    if (m_isBegun) return;
    
    m_currentEffect = effect;
    m_currentTransform = transformMatrix;
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
    
    m_sprites.push_back({texture, position, scale, rotation, origin, color, depth, Rectangle(), false});
}

void SpriteBatch::draw(std::shared_ptr<Texture> texture, 
                       const Vector2& position, 
                       const Rectangle& sourceRect,
                       const Vector2& scale, 
                       float rotation, 
                       const Vector2& origin, 
                       const Color& color, 
                       float depth) {
    if (!m_isBegun || !texture) return;
    
    m_sprites.push_back({texture, position, scale, rotation, origin, color, depth, sourceRect, true});
}

void SpriteBatch::drawString(const std::shared_ptr<SpriteFont>& font,
                             std::string_view text,
                             const Vector2& position,
                             const Color& color,
                             float rotation,
                             const Vector2& origin,
                             const Vector2& scale,
                             float depth) {
    if (!m_isBegun || !font) return;

    Vector2 cursorOffset(0.0f, 0.0f);

    auto it = text.begin();
    auto end = text.end();
    while (it != end) {
        uint32_t codepoint = decodeNextUtf8(it, end);
        if (codepoint == 0) break;

        if (codepoint == '\n') {
            cursorOffset.x = 0.0f;
            cursorOffset.y += font->getSize();
            continue;
        }

        const GlyphInfo* glyph = font->getGlyph(codepoint);
        if (glyph) {
            Vector2 offset = cursorOffset + Vector2(glyph->offsetX, glyph->offsetY);

            // Apply rotation and scaling to the offset relative to the string origin
            if (rotation != 0.0f) {
                float cosR = std::cos(rotation);
                float sinR = std::sin(rotation);
                float rx = offset.x * scale.x * cosR - offset.y * scale.y * sinR;
                float ry = offset.x * scale.x * sinR + offset.y * scale.y * cosR;
                offset = Vector2(rx, ry);
            } else {
                offset.x *= scale.x;
                offset.y *= scale.y;
            }

            // Draw the glyph
            draw(font->getTexture(), position + offset - origin, glyph->sourceRect, scale, rotation, {0.0f, 0.0f}, color, depth);

            cursorOffset.x += glyph->advanceX;
        }
    }
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
    m_vertexBuffer.reserve(m_sprites.size() * 6);
    
    for (size_t i = 0; i <= m_sprites.size(); ++i) {
        if (i == m_sprites.size() || m_sprites[i].texture != currentTexture) {
            // Flush current batch
            if (!m_vertexBuffer.empty()) {
                m_renderer.drawBatched(currentTexture, m_vertexBuffer.data(), m_vertexBuffer.size(), m_currentEffect, m_currentTransform);
                m_vertexBuffer.clear();
            }
            
            if (i < m_sprites.size()) {
                currentTexture = m_sprites[i].texture;
            }
        }
        
        if (i < m_sprites.size()) {
            const auto& sprite = m_sprites[i];
            
            float srcW = sprite.useSourceRect ? sprite.sourceRect.width : static_cast<float>(sprite.texture->getWidth());
            float srcH = sprite.useSourceRect ? sprite.sourceRect.height : static_cast<float>(sprite.texture->getHeight());

            float w = srcW * sprite.scale.x;
            float h = srcH * sprite.scale.y;
            
            float ox = sprite.origin.x * w;
            float oy = sprite.origin.y * h;
            
            float c = 1.0f;
            float s = 0.0f;
            if (sprite.rotation != 0.0f) {
                c = std::cos(sprite.rotation);
                s = std::sin(sprite.rotation);
            }
            
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
            
            float u0 = 0.0f;
            float v0 = 0.0f;
            float u1 = 1.0f;
            float v1 = 1.0f;

            if (sprite.useSourceRect) {
                float texW = static_cast<float>(sprite.texture->getWidth());
                float texH = static_cast<float>(sprite.texture->getHeight());
                u0 = sprite.sourceRect.x / texW;
                v0 = sprite.sourceRect.y / texH;
                u1 = (sprite.sourceRect.x + sprite.sourceRect.width) / texW;
                v1 = (sprite.sourceRect.y + sprite.sourceRect.height) / texH;
            }

            // Push 6 vertices for the quad (TRIANGLELIST)
            m_vertexBuffer.push_back({p0.x, p0.y, u0, v0, r, g, b, a}); // Top-left
            m_vertexBuffer.push_back({p1.x, p1.y, u1, v0, r, g, b, a}); // Top-right
            m_vertexBuffer.push_back({p2.x, p2.y, u0, v1, r, g, b, a}); // Bottom-left

            m_vertexBuffer.push_back({p2.x, p2.y, u0, v1, r, g, b, a}); // Bottom-left
            m_vertexBuffer.push_back({p1.x, p1.y, u1, v0, r, g, b, a}); // Top-right
            m_vertexBuffer.push_back({p3.x, p3.y, u1, v1, r, g, b, a}); // Bottom-right
        }
    }
}

} // namespace thengine
