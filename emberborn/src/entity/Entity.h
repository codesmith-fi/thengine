#pragma once
#include "EntityType.h"

namespace emberborn {

class Entity {
public:
    Entity();
    virtual ~Entity() = default;

    int getX() const { return m_gridX; }
    void setX(int x) { m_gridX = x; }

    int getY() const { return m_gridY; }
    void setY(int y) { m_gridY = y; }

    int getHitPoints() const { return m_hitPoints; }
    void setHitPoints(int hp) { m_hitPoints = hp; }

    int getExperiencePoints() const { return m_experiencePoints; }
    void setExperiencePoints(int xp) { m_experiencePoints = xp; }

    EntityType getType() const { return m_type; }
    EntityType getEntityType() const { return m_type; }
    void setType(EntityType type) { m_type = type; }

    float getSpeed() const { return m_speed; }
    void setSpeed(float speed) { m_speed = speed; }

private:
    int m_gridX = 0;
    int m_gridY = 0;
    int m_hitPoints = 100;
    int m_experiencePoints = 0;
    EntityType m_type;
    float m_speed = 1.0f;
};

} // namespace emberborn
