#include "thengine/graphics/Texture.h"
#include "thengine/Renderer.h"
#include "thengine/DebugLogger.h"
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

std::shared_ptr<Texture> Texture::createRenderTarget(Renderer& renderer, int width, int height) {
    SDL_GPUDevice* device = renderer.m_device;
    SDL_Window* window = renderer.m_window;

    SDL_GPUTextureCreateInfo createInfo = {};
    createInfo.type = SDL_GPU_TEXTURETYPE_2D;
    createInfo.format = SDL_GetGPUSwapchainTextureFormat(device, window);
    createInfo.width = width;
    createInfo.height = height;
    createInfo.layer_count_or_depth = 1;
    createInfo.num_levels = 1;
    createInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    createInfo.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;

    SDL_GPUTexture* tex = SDL_CreateGPUTexture(device, &createInfo);
    if (!tex) {
        LOG_ERROR() << "Failed to create render target texture: " << SDL_GetError();
        return nullptr;
    }

    return std::shared_ptr<Texture>(new Texture(device, tex, width, height, "RenderTarget"));
}

} // namespace thengine
