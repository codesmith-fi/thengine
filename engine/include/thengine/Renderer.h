#pragma once

#include <memory>
#include <cstdint>
#include "thengine/primitives/Vector2.h"
#include "thengine/primitives/Color.h"

// Forward declarations for SDL types
struct SDL_GPUDevice;
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
    void fillRect(const Vector2& pos, const Vector2& size, const Color& color);
    void present();
    
    SDL_GPUDevice* getDevice() const { return m_device; }

private:
    SDL_Window* m_window;
    SDL_GPUDevice* m_device;
};

} // namespace thengine
