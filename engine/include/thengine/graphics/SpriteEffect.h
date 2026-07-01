#pragma once

#include <memory>
#include "thengine/graphics/LightTypes.h"

struct SDL_GPUGraphicsPipeline;

namespace thengine {

class Renderer;
class Shader;

class SpriteEffect {
    friend class Renderer;
public:
    SpriteEffect(Renderer& renderer, std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader, BlendMode blendMode = BlendMode::Alpha);
    virtual ~SpriteEffect();

    SDL_GPUGraphicsPipeline* getPipeline() const { return m_pipeline; }

protected:
    Renderer& m_renderer;
    std::shared_ptr<Shader> m_vertexShader;
    std::shared_ptr<Shader> m_fragmentShader;
    SDL_GPUGraphicsPipeline* m_pipeline = nullptr;
};

} // namespace thengine
