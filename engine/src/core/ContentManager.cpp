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
    // Check cache
    auto it = m_cache.find(path);
    if (it != m_cache.end()) {
        try {
            return std::any_cast<std::shared_ptr<Texture>>(it->second);
        } catch (const std::bad_any_cast&) {
            LOG_ERROR() << "Resource type mismatch for path: " << path;
            return nullptr;
        }
    }

    // Load new texture
    SDL_Renderer* rawRenderer = m_renderer.getRawRenderer();
    SDL_Texture* rawTexture = IMG_LoadTexture(rawRenderer, path.c_str());
    
    if (!rawTexture) {
        LOG_ERROR() << "Failed to load texture at " << path << ": " << SDL_GetError();
        return nullptr;
    }

    auto texture = std::make_shared<Texture>(rawTexture);
    m_cache[path] = texture;
    
    LOG_INFO() << "Loaded and cached texture: " << path;
    return texture;
}

} // namespace thengine
