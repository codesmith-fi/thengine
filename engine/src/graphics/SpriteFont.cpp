#include "thengine/graphics/SpriteFont.h"
#include <algorithm>

namespace thengine {

SpriteFont::SpriteFont(std::shared_ptr<Texture> texture, float size, const std::unordered_map<char, GlyphInfo>& glyphs)
    : m_texture(texture), m_size(size), m_glyphs(glyphs) {
}

const GlyphInfo* SpriteFont::getGlyph(char c) const {
    auto it = m_glyphs.find(c);
    if (it != m_glyphs.end()) {
        return &it->second;
    }
    // Try to return a fallback character
    it = m_glyphs.find('?');
    if (it != m_glyphs.end()) {
        return &it->second;
    }
    return nullptr;
}

Vector2 SpriteFont::measureString(const std::string& text) const {
    float lineWidth = 0.0f;
    float maxLineWidth = 0.0f;
    int lineCount = 1;

    for (char c : text) {
        if (c == '\n') {
            maxLineWidth = std::max(maxLineWidth, lineWidth);
            lineWidth = 0.0f;
            lineCount++;
            continue;
        }

        const GlyphInfo* glyph = getGlyph(c);
        if (glyph) {
            lineWidth += glyph->advanceX;
        }
    }
    maxLineWidth = std::max(maxLineWidth, lineWidth);
    float height = static_cast<float>(lineCount) * m_size;

    return Vector2(maxLineWidth, height);
}

} // namespace thengine
