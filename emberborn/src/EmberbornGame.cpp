#include "EmberbornGame.h"
#include "thengine/DebugLogger.h"
#include "thengine/graphics/Shader.h"
#include "thengine/Input.h"
#include "thengine/Renderer.h"

EmberbornGame::EmberbornGame(const std::string &title, int width, int height)
    : thengine::Game(title, width, height) {}

EmberbornGame::~EmberbornGame() = default;

void EmberbornGame::onInitialize() {
  LOG_INFO() << "EmberbornGame initialized!";
  m_content = std::make_unique<thengine::ContentManager>(getRenderer());
  m_spriteBatch = std::make_unique<thengine::SpriteBatch>(getRenderer());
}

void EmberbornGame::onLoadContent() {
  auto vert = m_content->load<::thengine::Shader>("assets/shaders/basic.vert");
  auto frag = m_content->load<::thengine::Shader>("assets/shaders/basic.frag");
  m_basicEffect = std::make_shared<::thengine::BasicEffect>(getRenderer(), vert, frag);

  m_font = m_content->loadFont("assets/fonts/DejaVuSansMono.ttf", 36.0f);
}

bool EmberbornGame::onUpdate(float deltaTime) {
  if (thengine::Input::isKeyPressed(thengine::Key::Escape)) {
    LOG_INFO() << "ESCAPE pressed, exiting Emberborn.";
    return false;
  }
  return true;
}

void EmberbornGame::onRender(float deltaTime) {
  // Clear the screen to a deep, dark atmospheric black/red color
  getRenderer().clear(15, 2, 2, 255);

  // Set ambient light to fully lit (white) and clear point lights so the text is fully bright
  m_basicEffect->setAmbientColor(thengine::Color(255, 255, 255, 255));
  m_basicEffect->setAmbientIntensity(1.0f);
  m_basicEffect->clearLights();

  if (m_font) {
    m_spriteBatch->begin(m_basicEffect, thengine::Matrix4::identity());

    std::string text = "Welcome... to hell";
    thengine::Vector2 size = m_font->measureString(text);
    // Center of 1280x768 window
    thengine::Vector2 position((1280.0f - size.x) * 0.5f, (768.0f - size.y) * 0.5f);

    // Deep red/crimson colored warning text
    m_spriteBatch->drawString(m_font, text, position, thengine::Color(200, 30, 30, 255));

    m_spriteBatch->end();
  }
}

void EmberbornGame::onReleaseContent() {}

void EmberbornGame::onShutdown() {}
