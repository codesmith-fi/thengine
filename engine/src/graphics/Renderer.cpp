#include "thengine/Renderer.h"
#include "thengine/DebugLogger.h"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>

namespace thengine {

Renderer::Renderer(SDL_Window* window)
    : m_window(window), m_device(nullptr) {
    
    m_device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
        true, // debug_mode
        nullptr // name
    );

    if (!m_device) {
        LOG_ERROR() << "Failed to create SDL_GPUDevice: " << SDL_GetError();
        return;
    }

    if (!SDL_ClaimWindowForGPUDevice(m_device, m_window)) {
        LOG_ERROR() << "Failed to claim window for GPU Device: " << SDL_GetError();
        SDL_DestroyGPUDevice(m_device);
        m_device = nullptr;
        return;
    }

    LOG_INFO() << "SDL_GPUDevice initialized and window claimed successfully!";
}

Renderer::~Renderer() {
    if (m_device) {
        SDL_ReleaseWindowFromGPUDevice(m_device, m_window);
        SDL_DestroyGPUDevice(m_device);
    }
}

void Renderer::clear(uint8_t /*r*/, uint8_t /*g*/, uint8_t /*b*/, uint8_t /*a*/) {
    // Stubbed pending GPU pipeline setup
}

void Renderer::fillRect(const Vector2& /*pos*/, const Vector2& /*size*/, const Color& /*color*/) {
    // Stubbed pending GPU pipeline setup
}

void Renderer::present() {
    // Stubbed pending GPU pipeline setup
}

} // namespace thengine
