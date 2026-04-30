#pragma once

struct SDL_GPUDevice;
struct SDL_GPUTexture;

namespace thengine {

class Texture {
public:
    explicit Texture(SDL_GPUDevice* device, SDL_GPUTexture* texture);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;

    SDL_GPUTexture* getRaw() const { return m_texture; }

private:
    SDL_GPUDevice* m_device;
    SDL_GPUTexture* m_texture;
};

} // namespace thengine
