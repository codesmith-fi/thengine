#pragma once

#include "thengine/primitives/Rectangle.h"
#include "thengine/primitives/Vector2.h"
#include <memory>
#include <string_view>
#include <unordered_map>

namespace thengine {

class Texture;

struct GlyphInfo {
    Rectangle sourceRect;
    float offsetX;
    float offsetY;
    float advanceX;
};

class SpriteFont {
public:
    SpriteFont(std::shared_ptr<Texture> texture, float size, const std::unordered_map<uint32_t, GlyphInfo>& glyphs);
    ~SpriteFont() = default;

    [[nodiscard]] std::shared_ptr<Texture> getTexture() const noexcept { return m_texture; }
    [[nodiscard]] float getSize() const noexcept { return m_size; }
    [[nodiscard]] const GlyphInfo* getGlyph(uint32_t codepoint) const;

    [[nodiscard]] Vector2 measureString(std::string_view text) const;

private:
    std::shared_ptr<Texture> m_texture;
    float m_size;
    std::unordered_map<uint32_t, GlyphInfo> m_glyphs;
};

} // namespace thengine
