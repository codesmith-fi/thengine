#pragma once
#include <string>

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

    std::string getTypeKey() const { return m_typeKey; }
    void setTypeKey(const std::string& key) { m_typeKey = key; }

private:
    int m_gridX = 0;
    int m_gridY = 0;
    int m_hitPoints = 100;
    int m_experiencePoints = 0;
    std::string m_typeKey;
};

} // namespace emberborn
