#pragma once

#include <vector>
#include <memory>
#include <string>
#include "thengine/primitives/Vector2.h"
#include "thengine/primitives/Color.h"
#include "thengine/primitives/Rectangle.h"
#include "thengine/graphics/Vertex.h"
#include "thengine/math/Matrix4.h"

namespace thengine {

class Renderer;
class Texture;
class SpriteEffect;
class SpriteFont;

struct SpriteDrawCommand {
    std::shared_ptr<Texture> texture;
    Vector2 position;
    Vector2 scale;
    float rotation;
    Vector2 origin;
    Color color;
    float depth;
    Rectangle sourceRect;
    bool useSourceRect;
};

class SpriteBatch {
public:
    explicit SpriteBatch(Renderer& renderer);
    ~SpriteBatch();

    // Prevent copying
    SpriteBatch(const SpriteBatch&) = delete;
    SpriteBatch& operator=(const SpriteBatch&) = delete;

    void begin(std::shared_ptr<SpriteEffect> effect = nullptr, const Matrix4& transformMatrix = Matrix4::identity());
    
    void draw(std::shared_ptr<Texture> texture, 
              const Vector2& position, 
              const Vector2& scale = {1.0f, 1.0f}, 
              float rotation = 0.0f, 
              const Vector2& origin = {0.5f, 0.5f}, 
              const Color& color = {255, 255, 255, 255}, 
              float depth = 0.0f);

    void draw(std::shared_ptr<Texture> texture, 
              const Vector2& position, 
              const Rectangle& sourceRect,
              const Vector2& scale = {1.0f, 1.0f}, 
              float rotation = 0.0f, 
              const Vector2& origin = {0.5f, 0.5f}, 
              const Color& color = {255, 255, 255, 255}, 
              float depth = 0.0f);
              
    void drawString(const std::shared_ptr<SpriteFont>& font,
                    const std::string& text,
                    const Vector2& position,
                    const Color& color = {255, 255, 255, 255},
                    float rotation = 0.0f,
                    const Vector2& origin = {0.0f, 0.0f},
                    const Vector2& scale = {1.0f, 1.0f},
                    float depth = 0.0f);

    void end();

private:
    void flush();

    Renderer& m_renderer;
    std::vector<SpriteDrawCommand> m_sprites;
    std::vector<Vertex> m_vertexBuffer;
    std::shared_ptr<SpriteEffect> m_currentEffect;
    Matrix4 m_currentTransform;
    bool m_isBegun;
};

} // namespace thengine
