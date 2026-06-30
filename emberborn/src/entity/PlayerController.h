#pragma once
#include "EntityController.h"

namespace emberborn {

class PlayerController : public EntityController {
public:
    PlayerController();
    ~PlayerController() override = default;

    void update(float deltaTime, const TileMap& tileMap) override;

private:
    float m_moveCooldown = 0.0f;
    static constexpr float MOVE_COOLDOWN_TIME = 0.2f;
};

} // namespace emberborn
