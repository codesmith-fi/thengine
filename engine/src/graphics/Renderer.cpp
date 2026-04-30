#include "thengine/Renderer.h"
#include "thengine/DebugLogger.h"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_error.h>

namespace thengine {

Renderer::Renderer(SDL_Window* window)
    : m_renderer(nullptr, SDL_DestroyRenderer) {
    
    SDL_Renderer* rawRenderer = SDL_CreateRenderer(window, nullptr);
    if (!rawRenderer) {
        LOG_ERROR() << "Failed to create SDL Renderer: " << SDL_GetError();
    } else {
        m_renderer.reset(rawRenderer);
        const char* rendererName = SDL_GetRendererName(rawRenderer);
        LOG_INFO() << "Renderer created: " << (rendererName ? rendererName : "Unknown");
    }
}

Renderer::~Renderer() = default;

void Renderer::clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (m_renderer) {
        SDL_SetRenderDrawColor(m_renderer.get(), r, g, b, a);
        SDL_RenderClear(m_renderer.get());
    }
}

void Renderer::present() {
    if (m_renderer) {
        SDL_RenderPresent(m_renderer.get());
    }
}

} // namespace thengine
