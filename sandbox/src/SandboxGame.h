#pragma once

#include "thengine/Game.h"
#include <string>

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
};
