#pragma once

#include "thengine/Entity.h"
#include "thengine/primitives/Color.h"
#include <memory>

namespace thengine {

class Texture;

class Sprite : public Entity {
public:
    Sprite();
    ~Sprite() override;

    void render(SpriteBatch& batch) override;

    void setOrigin(const Vector2& origin) { m_origin = origin; }
    [[nodiscard]] const Vector2& getOrigin() const { return m_origin; }

    void setColor(const Color& color) { m_color = color; }
    [[nodiscard]] const Color& getColor() const { return m_color; }

    void setTexture(std::shared_ptr<Texture> texture) { m_texture = texture; }
    [[nodiscard]] std::shared_ptr<Texture> getTexture() const { return m_texture; }

    void setDepth(float depth) { m_depth = depth; }
    [[nodiscard]] float getDepth() const { return m_depth; }

private:
    Color m_color;
    std::shared_ptr<Texture> m_texture;
    Vector2 m_origin;
    float m_depth;
};

} // namespace thengine
