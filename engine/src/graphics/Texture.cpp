#include "thengine/graphics/Texture.h"
#include <SDL3/SDL_gpu.h>

namespace thengine {

Texture::Texture(SDL_GPUDevice* device, SDL_GPUTexture* texture, int width, int height, const std::string& path)
    : m_device(device), m_texture(texture), m_width(width), m_height(height), m_path(path) {
}

Texture::~Texture() {
    if (m_device && m_texture) {
        SDL_ReleaseGPUTexture(m_device, m_texture);
    }
}

} // namespace thengine
