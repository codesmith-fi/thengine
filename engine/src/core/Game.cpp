#include "thengine/Game.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <iostream>
#include "thengine/DebugLogger.h"

namespace thengine {

void SDLWindowDeleter::operator()(SDL_Window *window) const {
  if (window) {
    SDL_DestroyWindow(window);
  }
}

// Internal RAII helper for SDL subsystems
struct SDLContext {
  SDLContext() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
      LOG_ERROR() << "Failed to initialize SDL3: " << SDL_GetError();
    }
  }
  ~SDLContext() { SDL_Quit(); }
};

Game::Game(const std::string &title, int width, int height)
    : m_title(title), m_width(width), m_height(height), m_isRunning(false),
      m_fpsLimit(-1), m_deltaTime(0.0f), m_totalGameTime(0.0f),
      m_realizedFPS(0.0f) {}

Game::~Game() = default;

float Game::getDeltaTime() const noexcept { return m_deltaTime; }
float Game::getTotalGameTime() const noexcept { return m_totalGameTime; }
float Game::getRealizedFPS() const noexcept { return m_realizedFPS; }
void Game::setFramerateLimit(int fps) noexcept { m_fpsLimit = fps; }

// Empty implementations for lifecycle hooks
void Game::onInitialize() {}
void Game::onLoadContent() {}
void Game::onUpdate(float /*deltaTime*/) {}
void Game::onRender(float /*deltaTime*/) {}
void Game::onReleaseContent() {}
void Game::onShutdown() {}

int Game::run() {
  // RAII SDL init/quit
  SDLContext sdlContext;

  m_window.reset(SDL_CreateWindow(m_title.c_str(), m_width, m_height, 0));
  if (!m_window) {
    LOG_ERROR() << "Failed to create SDL window: " << SDL_GetError();
    return -1;
  }

  onInitialize();
  onLoadContent();

  m_isRunning = true;
  Uint64 lastTimeMs = SDL_GetTicks();
  Uint64 fpsTimerMs = lastTimeMs;
  Uint32 frameCount = 0;

  while (m_isRunning) {
    Uint64 currentTimeMs = SDL_GetTicks();
    float dt = static_cast<float>(currentTimeMs - lastTimeMs) / 1000.0f;
    lastTimeMs = currentTimeMs;

    m_deltaTime = dt;
    m_totalGameTime = static_cast<float>(currentTimeMs);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        m_isRunning = false;
      }
    }

    onUpdate(dt);
    onRender(dt);

    // Frame timing and limiting
    frameCount++;
    if (currentTimeMs - fpsTimerMs >= 1000) {
      m_realizedFPS = static_cast<float>(frameCount);
      frameCount = 0;
      fpsTimerMs = currentTimeMs;
    }

    if (m_fpsLimit > 0) {
      float targetFrameTimeMs = 1000.0f / static_cast<float>(m_fpsLimit);
      Uint64 elapsedTimeMs = SDL_GetTicks() - currentTimeMs;

      if (static_cast<float>(elapsedTimeMs) < targetFrameTimeMs) {
        SDL_Delay(static_cast<Uint32>(targetFrameTimeMs - elapsedTimeMs));
      }

      // Re-read time after delay to keep dt accurate for next frame
      lastTimeMs = SDL_GetTicks();
    }
  }

  onReleaseContent();
  onShutdown();

  return 0;
}

} // namespace thengine
