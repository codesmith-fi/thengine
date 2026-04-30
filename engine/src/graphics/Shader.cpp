#include "thengine/graphics/Shader.h"
#include <SDL3/SDL_gpu.h>

namespace thengine {

Shader::Shader(SDL_GPUDevice* device, SDL_GPUShader* shader)
    : m_device(device), m_shader(shader) {
}

Shader::~Shader() {
    if (m_device && m_shader) {
        SDL_ReleaseGPUShader(m_device, m_shader);
    }
}

} // namespace thengine
