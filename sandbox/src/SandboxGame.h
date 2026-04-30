#pragma once

#include "thengine/Game.h"
#include "thengine/graphics/Sprite.h"
#include "thengine/ContentManager.h"
#include "thengine/graphics/Texture.h"
#include <string>
#include <memory>

class SandboxGame : public thengine::Game {
public:
    SandboxGame(const std::string& title, int width, int height);
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
    
    std::unique_ptr<thengine::ContentManager> m_content;
    std::shared_ptr<thengine::Texture> m_testTexture;
};
