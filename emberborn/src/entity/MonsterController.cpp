#include "MonsterController.h"
#include "Entity.h"
#include "GenericNeutralEntityAI.h"

namespace emberborn {

MonsterController::MonsterController() : EntityController() {
    setAIStrategy(std::make_unique<GenericNeutralEntityAI>());
}

void MonsterController::update(float deltaTime, const TileMap& tileMap) {
    if (m_possessedEntity && m_aiStrategy) {
        m_aiStrategy->determineAction(deltaTime, *this, *m_possessedEntity, tileMap);
    }
}

} // namespace emberborn
