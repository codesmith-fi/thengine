#pragma once

#include <string>
#include <memory>

// Forward declarations for SDL types
struct SDL_Window;

namespace thengine {

class Renderer;

struct SDLWindowDeleter {
    void operator()(SDL_Window* window) const;
};

class Game {
public:
    Game(const std::string& title = "thengine Game", int width = 800, int height = 600);
    virtual ~Game();

    // Prevent copy/move
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;

    // Core loop
    int run();

    // Configuration
    void setFramerateLimit(int fps) noexcept;

    // State getters
    [[nodiscard]] float getDeltaTime() const noexcept;
    [[nodiscard]] float getTotalGameTime() const noexcept;
    [[nodiscard]] float getRealizedFPS() const noexcept;

protected:
    virtual void onInitialize();
    virtual void onLoadContent();
    virtual bool onUpdate(float deltaTime);
    virtual void onRender(float deltaTime);
    virtual void onReleaseContent();
    virtual void onShutdown();

    Renderer& getRenderer() { return *m_renderer; }

private:
    std::string m_title;
    int m_width;
    int m_height;
    bool m_isRunning;
    
    int m_fpsLimit;
    float m_deltaTime;
    float m_totalGameTime;
    float m_realizedFPS;

    std::unique_ptr<SDL_Window, SDLWindowDeleter> m_window;
    std::unique_ptr<Renderer> m_renderer;
};

} // namespace thengine
