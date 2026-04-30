#include "thengine/graphics/Texture.h"
#include <SDL3/SDL_render.h>

namespace thengine {

Texture::Texture(SDL_Texture* texture)
    : m_texture(texture, SDL_DestroyTexture) {
}

Texture::~Texture() = default;

} // namespace thengine
