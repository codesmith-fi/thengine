#include "thengine/graphics/BasicEffect.h"

namespace thengine {

BasicEffect::BasicEffect(Renderer& renderer, std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader)
    : SpriteEffect(renderer, vertexShader, fragmentShader) {
}

} // namespace thengine
