#pragma once

#include "thengine/graphics/SpriteEffect.h"
#include "thengine/graphics/Vertex.h"
#include "thengine/primitives/Color.h"
#include "thengine/primitives/Vector2.h"
#include "thengine/math/Matrix4.h"

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

// Forward declarations for SDL types
struct SDL_GPUDevice;
struct SDL_Window;
struct SDL_GPUCommandBuffer;
struct SDL_GPUTexture;
struct SDL_GPURenderPass;
struct SDL_GPUGraphicsPipeline;
struct SDL_GPUBuffer;
struct SDL_GPUSampler;
struct SDL_GPUShader;
struct SDL_GPUTransferBuffer;

namespace thengine {

class Texture;

class Renderer {
  friend class Game;
  friend class ContentManager;
  friend class Texture;
  friend class Shader;
  friend class SpriteEffect;

public:
  ~Renderer();

  // Prevent copy/move
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;
  Renderer(Renderer &&) = delete;
  Renderer &operator=(Renderer &&) = delete;

  bool beginFrame();
  void endFrame();

  void clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  void fillRect(const Vector2 &pos, const Vector2 &size, const Color &color);

  // Support for SpriteBatch drawing
  void drawBatched(std::shared_ptr<Texture> texture, const Vertex *vertices,
                   size_t vertexCount, std::shared_ptr<SpriteEffect> effect = nullptr,
                   const Matrix4& transformMatrix = Matrix4::identity());

  // Effect registration (required for sprite effects)
  void registerEffect(int id, std::shared_ptr<SpriteEffect> effect);

  // Helper for compiling a graphics pipeline compatible with standard 2D sprite rendering
  SDL_GPUGraphicsPipeline* createGraphicsPipeline(SDL_GPUShader* vertShader, SDL_GPUShader* fragShader);

private:
  explicit Renderer(SDL_Window *window);

  SDL_GPUDevice *getDevice() const { return m_device; }

  SDL_Window *m_window;
  SDL_GPUDevice *m_device;

  SDL_GPUCommandBuffer *m_cmdBuf = nullptr;
  SDL_GPUTexture *m_swapchainTexture = nullptr;
  SDL_GPURenderPass *m_renderPass = nullptr;

  static constexpr size_t NUM_FRAMES = 3;
  static constexpr size_t MAX_VERTICES_PER_FRAME = 60000;
  static constexpr size_t MAX_VERTICES = NUM_FRAMES * MAX_VERTICES_PER_FRAME;

  SDL_GPUGraphicsPipeline *m_spritePipeline = nullptr;
  SDL_GPUBuffer *m_vertexBuffer = nullptr;
  SDL_GPUTransferBuffer *m_transferBuffer = nullptr;
  SDL_GPUSampler *m_sampler = nullptr;
  size_t m_vertexOffset = 0;
  size_t m_currentFrameIndex = 0;

  struct RenderBatch {
    std::shared_ptr<Texture> texture;
    size_t startVertex;
    size_t vertexCount;
    std::shared_ptr<SpriteEffect> effect;
    Matrix4 transform;
  };

  std::vector<Vertex> m_frameVertices;
  std::vector<RenderBatch> m_frameBatches;

  std::unordered_map<int, std::shared_ptr<SpriteEffect>> m_Effects;

  void initPipeline();
  void cleanupPipeline();
};

} // namespace thengine
