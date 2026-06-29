#include "thengine/graphics/SpriteFont.h"
#include <algorithm>

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

SpriteFont::SpriteFont(std::shared_ptr<Texture> texture, float size, const std::unordered_map<uint32_t, GlyphInfo>& glyphs)
    : m_texture(texture), m_size(size), m_glyphs(glyphs) {
}

const GlyphInfo* SpriteFont::getGlyph(uint32_t codepoint) const {
    auto it = m_glyphs.find(codepoint);
    if (it != m_glyphs.end()) {
        return &it->second;
    }
    // Try fallback
    it = m_glyphs.find('?');
    if (it != m_glyphs.end()) {
        return &it->second;
    }
    return nullptr;
}

Vector2 SpriteFont::measureString(std::string_view text) const {
    float lineWidth = 0.0f;
    float maxLineWidth = 0.0f;
    int lineCount = 1;

    auto it = text.begin();
    auto end = text.end();
    while (it != end) {
        uint32_t codepoint = decodeNextUtf8(it, end);
        if (codepoint == 0) break;

        if (codepoint == '\n') {
            maxLineWidth = std::max(maxLineWidth, lineWidth);
            lineWidth = 0.0f;
            lineCount++;
            continue;
        }

        const GlyphInfo* glyph = getGlyph(codepoint);
        if (glyph) {
            lineWidth += glyph->advanceX;
        }
    }
    maxLineWidth = std::max(maxLineWidth, lineWidth);
    float height = static_cast<float>(lineCount) * m_size;

    return Vector2(maxLineWidth, height);
}

} // namespace thengine
