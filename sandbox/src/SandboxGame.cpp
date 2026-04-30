#include "SandboxGame.h"
#include "thengine/DebugLogger.h"
#include "thengine/Input.h"
#include "thengine/Renderer.h"

SandboxGame::SandboxGame(const std::string& title, int width, int height)
    : thengine::Game(title, width, height),
      m_hasLoggedUpdate(false),
      m_hasLoggedRender(false) {
      
    m_player.m_position = thengine::Vector2(width / 2.0f, height / 2.0f);
    m_player.m_scale = thengine::Vector2(50.0f, 50.0f);
    m_player.m_color = thengine::Color(255, 0, 0, 255); // Red square
}

SandboxGame::~SandboxGame() = default;

void SandboxGame::onInitialize() {
    LOG_INFO() << "SandboxGame initialized!";
}

void SandboxGame::onLoadContent() {
    LOG_INFO() << "SandboxGame loading content!";
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

    float speed = 200.0f * deltaTime;
    if (thengine::Input::isKeyPressed(thengine::Key::W)) {
        m_player.m_position.y -= speed;
    }
    if (thengine::Input::isKeyPressed(thengine::Key::S)) {
        m_player.m_position.y += speed;
    }
    if (thengine::Input::isKeyPressed(thengine::Key::A)) {
        m_player.m_position.x -= speed;
    }
    if (thengine::Input::isKeyPressed(thengine::Key::D)) {
        m_player.m_position.x += speed;
    }

    return true;
}

void SandboxGame::onRender(float deltaTime) {
    if (!m_hasLoggedRender) {
        LOG_INFO() << "SandboxGame rendering! First frame delta time: " << deltaTime;
        m_hasLoggedRender = true;
    }

    getRenderer().clear(100, 149, 237, 255);
    m_player.render(getRenderer());
    getRenderer().present();
}

void SandboxGame::onReleaseContent() {
    LOG_INFO() << "SandboxGame releasing content!";
}

void SandboxGame::onShutdown() {
    LOG_INFO() << "SandboxGame shutting down!";
}
