#pragma once

#include <string>
#include <memory>

struct SDL_GPUDevice;
struct SDL_GPUTexture;

namespace thengine {

class Renderer;

class Texture {
  friend class Renderer;
  friend class ContentManager;

public:
  ~Texture();

  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;
  Texture(Texture &&) = delete;
  Texture &operator=(Texture &&) = delete;

  [[nodiscard]] int getWidth() const noexcept { return m_width; }
  [[nodiscard]] int getHeight() const noexcept { return m_height; }
  [[nodiscard]] const std::string &getPath() const noexcept { return m_path; }

  static std::shared_ptr<Texture> createRenderTarget(Renderer& renderer, int width, int height);

private:
  explicit Texture(SDL_GPUDevice *device, SDL_GPUTexture *texture, int width,
                   int height, const std::string &path);

  SDL_GPUDevice *m_device;
  SDL_GPUTexture *m_texture;
  int m_width;
  int m_height;
  std::string m_path;
};

} // namespace thengine
