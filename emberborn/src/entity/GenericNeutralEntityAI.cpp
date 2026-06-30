#include "GenericNeutralEntityAI.h"
#include "Entity.h"
#include "core/TileMap.h"
#include <cmath>
#include <random>

namespace emberborn {

GenericNeutralEntityAI::GenericNeutralEntityAI()
    : m_actionTimer(0.0f)
    , m_spawnX(-1)
    , m_spawnY(-1)
    , m_maxRadius(4) {}

void GenericNeutralEntityAI::determineAction(float deltaTime, EntityController& controller, Entity& possessedEntity, const TileMap& tileMap) {
    if (m_spawnX == -1 || m_spawnY == -1) {
        m_spawnX = possessedEntity.getX();
        m_spawnY = possessedEntity.getY();
    }

    m_actionTimer += deltaTime;

    float speed = possessedEntity.getSpeed();
    if (speed <= 0.0f) {
        speed = 1.0f; // safety baseline fallback
    }

    float interval = 1.5f / speed;
    if (m_actionTimer >= interval) {
        m_actionTimer = 0.0f;

        // Pick a random adjacent direction
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dirDist(0, 3);
        int direction = dirDist(gen);

        int dx = 0;
        int dy = 0;
        switch (direction) {
            case 0: dy = -1; break; // Up
            case 1: dy = 1;  break; // Down
            case 2: dx = -1; break; // Left
            case 3: dx = 1;  break; // Right
        }

        int targetX = possessedEntity.getX() + dx;
        int targetY = possessedEntity.getY() + dy;

        // Boundary & Radius Validation
        if (tileMap.inBounds(targetX, targetY)) {
            if (std::abs(targetX - m_spawnX) <= m_maxRadius && std::abs(targetY - m_spawnY) <= m_maxRadius) {
                Tile::TileType type = tileMap.getTile(targetX, targetY).type;
                if (type != Tile::TileType::Wall && type != Tile::TileType::Void) {
                    possessedEntity.setX(targetX);
                    possessedEntity.setY(targetY);
                }
            }
        }
    }
}

} // namespace emberborn
