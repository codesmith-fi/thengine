#include "thengine/graphics/Texture.h"
#include <SDL3/SDL_gpu.h>

namespace thengine {

Texture::Texture(SDL_GPUDevice* device, SDL_GPUTexture* texture)
    : m_device(device), m_texture(texture) {
}

Texture::~Texture() {
    if (m_device && m_texture) {
        SDL_ReleaseGPUTexture(m_device, m_texture);
    }
}

} // namespace thengine
