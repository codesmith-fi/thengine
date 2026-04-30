#pragma once

struct SDL_GPUDevice;
struct SDL_GPUShader;

namespace thengine {

class Shader {
public:
    explicit Shader(SDL_GPUDevice* device, SDL_GPUShader* shader);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(Shader&&) = delete;

    SDL_GPUShader* getRaw() const { return m_shader; }

private:
    SDL_GPUDevice* m_device;
    SDL_GPUShader* m_shader;
};

} // namespace thengine
