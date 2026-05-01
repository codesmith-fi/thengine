#pragma once

#include <memory>
#include <cstdint>
#include "thengine/primitives/Vector2.h"
#include "thengine/primitives/Color.h"
#include "thengine/graphics/Vertex.h"

// Forward declarations for SDL types
struct SDL_GPUDevice;
struct SDL_Window;
struct SDL_GPUCommandBuffer;
struct SDL_GPUTexture;
struct SDL_GPURenderPass;
struct SDL_GPUGraphicsPipeline;
struct SDL_GPUBuffer;
struct SDL_GPUSampler;

namespace thengine {

class Texture;

class Renderer {
    friend class Game;
    friend class ContentManager;
    friend class Texture;
    friend class Shader;

public:
    ~Renderer();

    // Prevent copy/move
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    bool beginFrame();
    void endFrame();
    
    void clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void fillRect(const Vector2& pos, const Vector2& size, const Color& color);
    
    // Support for SpriteBatch drawing
    void drawBatched(std::shared_ptr<Texture> texture, const Vertex* vertices, size_t vertexCount);

private:
    explicit Renderer(SDL_Window* window);
    
    SDL_GPUDevice* getDevice() const { return m_device; }

    SDL_Window* m_window;
    SDL_GPUDevice* m_device;
    
    SDL_GPUCommandBuffer* m_cmdBuf = nullptr;
    SDL_GPUTexture* m_swapchainTexture = nullptr;
    SDL_GPURenderPass* m_renderPass = nullptr;
    
    SDL_GPUGraphicsPipeline* m_spritePipeline = nullptr;
    SDL_GPUBuffer* m_vertexBuffer = nullptr;
    SDL_GPUSampler* m_sampler = nullptr;
    
    void initPipeline();
    void cleanupPipeline();
};

} // namespace thengine
