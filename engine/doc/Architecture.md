# thengine Architecture & Core Design

## Vision
thengine is a lightweight, modern, modular C++17 game engine focused on simplicity, performance and extensibility. It provides extendable classes usable by concrete game applications.

## Core Philosophy
- "Batteries included but removable"
- Easy to extend and inherit from
- Minimal boilerplate for the end user (thesandbox & future games)

## Core Class: Game

The central class of the engine is `thengine::Game`.

**Responsibilities:**
- Initialize all engine subsystems (window, renderer, input, audio, etc.)
- Manages and provides access to common resources and subsystems needed by the game applications, e.g. the graphics context, asset manager etc.
- Run the main game loop, either with fixed timestep (set FPS limit) or freely as fast as possible, controllable with `setFramerateLimit()` method (for unlimited framerate use -1 for example).
- Keeps track of total game time (in milliseconds) and provides methods to get it as milliseconds and realized FPS (Frames Per Seconds): `getDeltaTime()`, `getTotalGameTime()`, `getRealizedFPS()`,  
- Handle lifecycle: `onInitialize()`, `onLoadContent()`, `OnUpdate()`, `OnRender()`, `onReleaseContent()`, `OnShutdown()`. Base class has empty implementations for those.
- Provide clean hooks for game-specific logic through inheritance

### Usage Example (desired end result)

```cpp
class MyGame : public thengine::Game {
public:
    MyGame();
    
protected:
    void onInitialize() override;
    void onLoadContent() override;
    void onUpdate(float deltaTime) override;
    void onRender(float deltaTime) override;
    // void onReleaseContent() override - not implemented here
    void onShutdown() override;
};

int main() {
    MyGame game;
    return game.Run();
}
