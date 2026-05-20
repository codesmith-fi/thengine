#pragma once

#include "thengine/ContentManager.h"
#include "thengine/Game.h"
#include "thengine/graphics/BasicEffect.h"
#include "Camera2D.h"
#include "thengine/graphics/Sprite.h"
#include "thengine/graphics/Texture.h"
#include "thengine/graphics/SpriteBatch.h"
#include <array>
#include <memory>
#include <string>

constexpr int MAX_SPRITES = 100;
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 768;

class SandboxGame : public thengine::Game {
public:
  SandboxGame(const std::string &title, int width, int height);
  ~SandboxGame() override;

protected:
  void onInitialize() override;
  void onLoadContent() override;
  bool onUpdate(float deltaTime) override;
  void onRender(float deltaTime) override;
  void onReleaseContent() override;
  void onShutdown() override;

private:
  bool m_hasLoggedUpdate;
  bool m_hasLoggedRender;
  thengine::Sprite m_player;
  std::array<thengine::Sprite, MAX_SPRITES> m_sprites;

  std::unique_ptr<thengine::SpriteBatch> m_spriteBatch;
  std::unique_ptr<thengine::ContentManager> m_content;
  std::shared_ptr<thengine::Texture> m_testTexture1;
  std::shared_ptr<thengine::Texture> m_testTexture2;
  std::shared_ptr<thengine::Texture> m_testTexture3;
  std::shared_ptr<thengine::Texture> m_playerTexture;
  std::shared_ptr<thengine::BasicEffect> m_basicEffect;
  thengine::Camera2D m_camera;
};
