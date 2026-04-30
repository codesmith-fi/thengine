#pragma once

#include <memory>

struct SDL_Texture;

namespace thengine {

class Texture {
public:
    explicit Texture(SDL_Texture* texture);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;

    SDL_Texture* getRaw() const { return m_texture.get(); }

private:
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> m_texture;
};

} // namespace thengine
