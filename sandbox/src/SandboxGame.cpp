#include "SandboxGame.h"
#include "thengine/DebugLogger.h"
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_scancode.h>

SandboxGame::SandboxGame(const std::string& title, int width, int height)
    : thengine::Game(title, width, height),
      m_hasLoggedUpdate(false),
      m_hasLoggedRender(false) {
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

    const bool* state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_ESCAPE]) {
        LOG_INFO() << "ESCAPE pressed, exiting sandbox.";
        return false;
    }

    return true;
}

void SandboxGame::onRender(float deltaTime) {
    if (!m_hasLoggedRender) {
        LOG_INFO() << "SandboxGame rendering! First frame delta time: " << deltaTime;
        m_hasLoggedRender = true;
    }
}

void SandboxGame::onReleaseContent() {
    LOG_INFO() << "SandboxGame releasing content!";
}

void SandboxGame::onShutdown() {
    LOG_INFO() << "SandboxGame shutting down!";
}
