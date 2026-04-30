#pragma once

#include <memory>
#include <cstdint>

// Forward declarations for SDL types
struct SDL_Renderer;
struct SDL_Window;

namespace thengine {

class Renderer {
public:
    explicit Renderer(SDL_Window* window);
    ~Renderer();

    // Prevent copy/move
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void present();

private:
    std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> m_renderer;
};

} // namespace thengine
