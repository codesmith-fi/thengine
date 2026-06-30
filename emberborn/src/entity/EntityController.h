#pragma once
#include <memory>

#include "EntityAI.h"

namespace emberborn {

class Entity;
class TileMap;

class EntityController {
public:
    EntityController();
    virtual ~EntityController() = default;

    void possess(Entity* entity) { m_possessedEntity = entity; }
    Entity* getPossessedEntity() const { return m_possessedEntity; }

    void setAIStrategy(std::unique_ptr<EntityAI> newAI) { m_aiStrategy = std::move(newAI); }
    EntityAI* getAIStrategy() const { return m_aiStrategy.get(); }

    virtual void update(float deltaTime, const TileMap& tileMap) = 0;

protected:
    Entity* m_possessedEntity = nullptr;
    std::unique_ptr<EntityAI> m_aiStrategy;
};

} // namespace emberborn
