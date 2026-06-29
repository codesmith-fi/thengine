#pragma once

#include "thengine/graphics/SpriteEffect.h"
#include "thengine/primitives/Color.h"

namespace thengine {

class BasicEffect : public SpriteEffect {
public:
    BasicEffect(Renderer& renderer, std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader);
    ~BasicEffect() override = default;

    void setAmbientColor(const Color& color) noexcept;
    [[nodiscard]] Color getAmbientColor() const noexcept;

    void setAmbientIntensity(float intensity) noexcept;
    [[nodiscard]] float getAmbientIntensity() const noexcept;

private:
    Color m_ambientColor = Color(255, 255, 255, 255);
    float m_ambientIntensity = 1.0f;
};

} // namespace thengine
