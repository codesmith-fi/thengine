#include "thengine/graphics/BasicEffect.h"

namespace thengine {

BasicEffect::BasicEffect(Renderer& renderer, std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader)
    : SpriteEffect(renderer, vertexShader, fragmentShader) {
}

void BasicEffect::setAmbientColor(const Color& color) noexcept {
    m_ambientColor = color;
}

Color BasicEffect::getAmbientColor() const noexcept {
    return m_ambientColor;
}

void BasicEffect::setAmbientIntensity(float intensity) noexcept {
    m_ambientIntensity = intensity;
}

float BasicEffect::getAmbientIntensity() const noexcept {
    return m_ambientIntensity;
}

void BasicEffect::clearLights() noexcept {
    m_pointLights.clear();
}

void BasicEffect::addPointLight(const PointLight& light) noexcept {
    if (m_pointLights.size() < 10) {
        m_pointLights.push_back(light);
    }
}

const std::vector<PointLight>& BasicEffect::getPointLights() const noexcept {
    return m_pointLights;
}

} // namespace thengine
