#pragma once

struct SDL_GPUDevice;
struct SDL_GPUShader;

namespace thengine {

class Shader {
    friend class Renderer;
    friend class ContentManager;
    friend class SpriteEffect;

public:
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(Shader&&) = delete;

private:
    explicit Shader(SDL_GPUDevice* device, SDL_GPUShader* shader);

    SDL_GPUDevice* m_device;
    SDL_GPUShader* m_shader;
};

} // namespace thengine
