#pragma once
#include "EntityAI.h"

namespace emberborn {

class GenericNeutralEntityAI : public EntityAI {
public:
    GenericNeutralEntityAI();
    ~GenericNeutralEntityAI() override = default;

    void determineAction(float deltaTime, EntityController& controller, Entity& possessedEntity, const TileMap& tileMap) override;

private:
    float m_actionTimer = 0.0f;
    int m_spawnX = -1;
    int m_spawnY = -1;
    int m_maxRadius = 4;
};

} // namespace emberborn
