#pragma once

#include "thengine/graphics/SpriteEffect.h"
#include "thengine/graphics/LightTypes.h"
#include "thengine/primitives/Color.h"
#include <vector>

namespace thengine {

class BasicEffect : public SpriteEffect {
public:
    BasicEffect(Renderer& renderer, std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader);
    ~BasicEffect() override = default;

    void setAmbientColor(const Color& color) noexcept;
    [[nodiscard]] Color getAmbientColor() const noexcept;

    void setAmbientIntensity(float intensity) noexcept;
    [[nodiscard]] float getAmbientIntensity() const noexcept;

    void clearLights() noexcept;
    void addPointLight(const PointLight& light) noexcept;
    [[nodiscard]] const std::vector<PointLight>& getPointLights() const noexcept;

private:
    Color m_ambientColor = Color(255, 255, 255, 255);
    float m_ambientIntensity = 1.0f;

    std::vector<PointLight> m_pointLights;
};

} // namespace thengine
