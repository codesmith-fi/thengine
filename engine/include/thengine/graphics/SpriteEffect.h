#pragma once

#include <memory>

struct SDL_GPUGraphicsPipeline;

namespace thengine {

class Renderer;
class Shader;

class SpriteEffect {
    friend class Renderer;
public:
    SpriteEffect(Renderer& renderer, std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader);
    virtual ~SpriteEffect();

    SDL_GPUGraphicsPipeline* getPipeline() const { return m_pipeline; }

private:
    Renderer& m_renderer;
    std::shared_ptr<Shader> m_vertexShader;
    std::shared_ptr<Shader> m_fragmentShader;
    SDL_GPUGraphicsPipeline* m_pipeline = nullptr;
};

} // namespace thengine
