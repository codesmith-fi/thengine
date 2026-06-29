#include "SandboxGame.h"
#include "thengine/DebugLogger.h"
#include "thengine/graphics/Shader.h"
#include "thengine/Input.h"
#include "thengine/Renderer.h"
#include <algorithm>

SandboxGame::SandboxGame(const std::string &title, int width, int height)
    : thengine::Game(title, width, height), m_hasLoggedUpdate(false),
      m_hasLoggedRender(false) {

  m_player.setPosition(thengine::Vector2(width / 2.0f, height / 2.0f));
  m_player.setScale(thengine::Vector2(0.1f, 0.1f));
  m_player.setDepth(2.0f); // Set depth high so it is drawn last (on top)
  m_player.setColor(
      thengine::Color(255, 255, 255, 255)); // White tint so texture is visible
}

SandboxGame::~SandboxGame() = default;

void SandboxGame::onInitialize() {
  LOG_INFO() << "SandboxGame initialized!";
  m_content = std::make_unique<thengine::ContentManager>(getRenderer());
  m_spriteBatch = std::make_unique<thengine::SpriteBatch>(getRenderer());
  m_camera.position = m_player.getPosition();
  m_camera.zoom = 1.0f;
  m_camera.rotation = 0.0f;
}

void SandboxGame::onLoadContent() {
  m_testTexture1 = m_content->load<thengine::Texture>("assets/test1.png");
  m_testTexture2 = m_content->load<thengine::Texture>("assets/test2.png");
  m_testTexture3 = m_content->load<thengine::Texture>("assets/test3.png");
  m_playerTexture = m_content->load<thengine::Texture>("assets/player.png");

  auto vert = m_content->load<::thengine::Shader>("assets/shaders/basic.vert");
  auto frag = m_content->load<::thengine::Shader>("assets/shaders/basic.frag");
  m_basicEffect = std::make_shared<::thengine::BasicEffect>(getRenderer(), vert, frag);

  srand(time(NULL));

  int currentTex = 0;
  float scale = 0.1f;
  for (int i = 0; i < MAX_SPRITES; i++) {
    int randomX = rand() % WINDOW_WIDTH;
    int randomY = rand() % WINDOW_HEIGHT;
    LOG_INFO() << "Sprite " << i << " at (" << randomX << ", " << randomY
               << ")";
               
    // Depth 0.0 (furthest) - 1.0 (closest)
    float depth = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    // Scale is based on depth (smaller depth = smaller size)
    scale = 0.05f + (depth * 0.25f);
    
    m_sprites[i].setDepth(depth);
    m_sprites[i].setPosition(thengine::Vector2(randomX, randomY));
    m_sprites[i].setScale(thengine::Vector2(scale, scale));
    m_sprites[i].setColor(thengine::Color(
        255, 255, 255, 255)); // White tint so texture is visible
    switch (currentTex) {
    case 0:
      m_sprites[i].setTexture(m_testTexture1);
      break;
    case 1:
      m_sprites[i].setTexture(m_testTexture2);
      break;
    case 2:
      m_sprites[i].setTexture(m_testTexture3);
      break;
    }
    currentTex = (currentTex + 1) % 3;
  }
  
  m_player.setTexture(m_playerTexture);

  m_debugFont = m_content->loadFont("assets/fonts/DejaVuSansMono.ttf", 24.0f);
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

  // Camera follows player
  m_camera.position = m_player.getPosition();

  // Zoom controls
  if (thengine::Input::isKeyPressed(thengine::Key::Up)) {
    m_camera.zoom += deltaTime * 1.0f;
  }
  if (thengine::Input::isKeyPressed(thengine::Key::Down)) {
    m_camera.zoom -= deltaTime * 1.0f;
    if (m_camera.zoom < 0.1f) m_camera.zoom = 0.1f;
  }

  // Camera rotation controls
  if (thengine::Input::isKeyPressed(thengine::Key::Left)) {
    m_camera.rotation -= deltaTime * 1.0f;
  }
  if (thengine::Input::isKeyPressed(thengine::Key::Right)) {
    m_camera.rotation += deltaTime * 1.0f;
  }

  return true;
}

void SandboxGame::onRender(float deltaTime) {
  if (!m_hasLoggedRender) {
    LOG_INFO() << "SandboxGame rendering! First frame delta time: "
               << deltaTime;
    m_hasLoggedRender = true;
  }

  getRenderer().clear(100, 149, 237, 255);

  thengine::Matrix4 cameraTransform = m_camera.getTransform(WINDOW_WIDTH, WINDOW_HEIGHT);
  m_spriteBatch->begin(m_basicEffect, cameraTransform);

  for (int i = 0; i < MAX_SPRITES; i++) {
    m_sprites[i].render(*m_spriteBatch);
  }
  m_player.render(*m_spriteBatch);

  // Draw text above player head in world space
  if (m_debugFont) {
    m_spriteBatch->drawString(m_debugFont, "Player", m_player.getPosition() - thengine::Vector2(40.0f, 60.0f), thengine::Color(0, 0, 0, 255));
  }

  m_spriteBatch->end();

  // Draw HUD overlay in screen space
  if (m_debugFont) {
    m_spriteBatch->begin(m_basicEffect, thengine::Matrix4::identity());

    char posBuffer[128];
    std::snprintf(posBuffer, sizeof(posBuffer), "Player Pos: (%.2f, %.2f)", m_player.getPosition().x, m_player.getPosition().y);
    m_spriteBatch->drawString(m_debugFont, posBuffer, thengine::Vector2(20.0f, 20.0f), thengine::Color(255, 255, 0, 255));

    char camBuffer[128];
    std::snprintf(camBuffer, sizeof(camBuffer), "Camera Zoom: %.2f | Rot: %.2f", m_camera.zoom, m_camera.rotation);
    m_spriteBatch->drawString(m_debugFont, camBuffer, thengine::Vector2(20.0f, 50.0f), thengine::Color(255, 255, 255, 255));

    m_spriteBatch->drawString(m_debugFont, "Controls: WASD=Move | Up/Down=Zoom | Left/Right=Rotate | ESC=Exit", 
                              thengine::Vector2(20.0f, static_cast<float>(WINDOW_HEIGHT) - 40.0f), thengine::Color(0, 255, 0, 255));

    m_spriteBatch->end();
  }
}

void SandboxGame::onReleaseContent() {
  LOG_INFO() << "SandboxGame releasing content!";
}

void SandboxGame::onShutdown() { LOG_INFO() << "SandboxGame shutting down!"; }
