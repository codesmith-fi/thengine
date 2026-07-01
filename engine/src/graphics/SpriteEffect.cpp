#include "thengine/graphics/SpriteEffect.h"
#include "thengine/graphics/Shader.h"
#include "thengine/Renderer.h"
#include <SDL3/SDL_gpu.h>

namespace thengine {

SpriteEffect::SpriteEffect(Renderer& renderer, std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader, BlendMode blendMode)
    : m_renderer(renderer), m_vertexShader(vertexShader), m_fragmentShader(fragmentShader) {
    if (m_vertexShader && m_fragmentShader) {
        m_pipeline = m_renderer.createGraphicsPipeline(m_vertexShader->m_shader, m_fragmentShader->m_shader, blendMode);
    }
}

SpriteEffect::~SpriteEffect() {
    if (m_pipeline) {
        SDL_ReleaseGPUGraphicsPipeline(m_renderer.getDevice(), m_pipeline);
        m_pipeline = nullptr;
    }
}

} // namespace thengine
