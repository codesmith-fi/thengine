#include "thengine/ContentManager.h"
#include "thengine/graphics/Texture.h"
#include "thengine/Renderer.h"
#include "thengine/DebugLogger.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_error.h>

namespace thengine {

ContentManager::ContentManager(Renderer& renderer)
    : m_renderer(renderer) {
}

template<>
std::shared_ptr<Texture> ContentManager::load<Texture>(const std::string& path) {
    LOG_INFO() << "Texture loading is currently stubbed out pending SDL_GPU pipeline setup (path: " << path << ")";
    return nullptr;
}

} // namespace thengine
