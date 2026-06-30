#pragma once

namespace emberborn {

class Entity;
class EntityController;
class TileMap;

class EntityAI {
public:
    virtual ~EntityAI() = default;
    virtual void determineAction(float deltaTime, EntityController& controller, Entity& possessedEntity, const TileMap& tileMap) = 0;
};

} // namespace emberborn
