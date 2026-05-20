#pragma once

#include "thengine/graphics/SpriteEffect.h"

namespace thengine {

class BasicEffect : public SpriteEffect {
public:
    BasicEffect(Renderer& renderer, std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader);
    ~BasicEffect() override = default;
};

} // namespace thengine
