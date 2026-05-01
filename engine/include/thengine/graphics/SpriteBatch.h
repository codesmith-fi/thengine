#pragma once

#include <vector>
#include <memory>
#include "thengine/primitives/Vector2.h"
#include "thengine/primitives/Color.h"
#include "thengine/graphics/Vertex.h"

namespace thengine {

class Renderer;
class Texture;
class SpriteEffect;

struct SpriteDrawCommand {
    std::shared_ptr<Texture> texture;
    Vector2 position;
    Vector2 scale;
    float rotation;
    Vector2 origin;
    Color color;
    float depth;
};

class SpriteBatch {
public:
    explicit SpriteBatch(Renderer& renderer);
    ~SpriteBatch();

    // Prevent copying
    SpriteBatch(const SpriteBatch&) = delete;
    SpriteBatch& operator=(const SpriteBatch&) = delete;

    void begin(std::shared_ptr<SpriteEffect> effect = nullptr);
    
    void draw(std::shared_ptr<Texture> texture, 
              const Vector2& position, 
              const Vector2& scale = {1.0f, 1.0f}, 
              float rotation = 0.0f, 
              const Vector2& origin = {0.5f, 0.5f}, 
              const Color& color = {255, 255, 255, 255}, 
              float depth = 0.0f);
              
    void end();

private:
    void flush();

    Renderer& m_renderer;
    std::vector<SpriteDrawCommand> m_sprites;
    std::vector<Vertex> m_vertexBuffer;
    std::shared_ptr<SpriteEffect> m_currentEffect;
    bool m_isBegun;
};

} // namespace thengine
