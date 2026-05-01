#include "SandboxGame.h"
#include "thengine/DebugLogger.h"
#include "thengine/Input.h"
#include "thengine/Renderer.h"

SandboxGame::SandboxGame(const std::string &title, int width, int height)
    : thengine::Game(title, width, height), m_hasLoggedUpdate(false),
      m_hasLoggedRender(false) {

  m_player.m_position = thengine::Vector2(width / 2.0f, height / 2.0f);
  m_player.m_scale = thengine::Vector2(0.1f, 0.1f);
  m_player.m_color =
      thengine::Color(255, 255, 255, 255); // White tint so texture is visible
}

SandboxGame::~SandboxGame() = default;

void SandboxGame::onInitialize() {
  LOG_INFO() << "SandboxGame initialized!";
  m_content = std::make_unique<thengine::ContentManager>(getRenderer());
}

void SandboxGame::onLoadContent() {
  m_testTexture = m_content->load<thengine::Texture>("assets/cross.png");
  m_player.m_texture = m_testTexture;
}

bool SandboxGame::onUpdate(float deltaTime) {
  if (!m_hasLoggedUpdate) {
    LOG_INFO() << "SandboxGame updating! First frame delta time: " << deltaTime;
    m_hasLoggedUpdate = true;
  }

  if (thengine::Input::isKeyPressed(thengine::Key::Escape)) {
    LOG_INFO() << "ESCAPE pressed, exiting sandbox.";
    return false;
  }

  thengine::Vector2 direction(0.0f, 0.0f);
  if (thengine::Input::isKeyPressed(thengine::Key::W)) {
    direction.y -= 1.0f;
  }
  if (thengine::Input::isKeyPressed(thengine::Key::S)) {
    direction.y += 1.0f;
  }
  if (thengine::Input::isKeyPressed(thengine::Key::A)) {
    direction.x -= 1.0f;
  }
  if (thengine::Input::isKeyPressed(thengine::Key::D)) {
    direction.x += 1.0f;
  }

  direction = direction.normalized();
  float speed = 200.0f;
  thengine::Vector2 velocity = direction * (speed * deltaTime);
  m_player.move(velocity);

  // Rotate slowly over time
  m_player.rotate(deltaTime * 2.0f);

  return true;
}

void SandboxGame::onRender(float deltaTime) {
  if (!m_hasLoggedRender) {
    LOG_INFO() << "SandboxGame rendering! First frame delta time: "
               << deltaTime;
    m_hasLoggedRender = true;
  }

  getRenderer().clear(100, 149, 237, 255);
  m_player.render(getRenderer());
}

void SandboxGame::onReleaseContent() {
  LOG_INFO() << "SandboxGame releasing content!";
}

void SandboxGame::onShutdown() { LOG_INFO() << "SandboxGame shutting down!"; }
