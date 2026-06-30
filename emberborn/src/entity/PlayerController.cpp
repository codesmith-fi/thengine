#include "PlayerController.h"
#include "Entity.h"
#include "core/TileMap.h"
#include "thengine/Input.h"

namespace emberborn {

PlayerController::PlayerController()
    : EntityController()
    , m_moveCooldown(0.0f) {}

void PlayerController::update(float deltaTime, const TileMap& tileMap) {
    if (m_moveCooldown > 0.0f) {
        m_moveCooldown -= deltaTime;
    }

    if (!m_possessedEntity || m_moveCooldown > 0.0f) {
        return;
    }

    int dx = 0;
    int dy = 0;

    if (thengine::Input::isKeyPressed(thengine::Key::W) || thengine::Input::isKeyPressed(thengine::Key::Up)) {
        dy = -1;
    } else if (thengine::Input::isKeyPressed(thengine::Key::S) || thengine::Input::isKeyPressed(thengine::Key::Down)) {
        dy = 1;
    } else if (thengine::Input::isKeyPressed(thengine::Key::A) || thengine::Input::isKeyPressed(thengine::Key::Left)) {
        dx = -1;
    } else if (thengine::Input::isKeyPressed(thengine::Key::D) || thengine::Input::isKeyPressed(thengine::Key::Right)) {
        dx = 1;
    }

    if (dx != 0 || dy != 0) {
        int targetX = m_possessedEntity->getX() + dx;
        int targetY = m_possessedEntity->getY() + dy;

        if (tileMap.inBounds(targetX, targetY)) {
            Tile::TileType type = tileMap.getTile(targetX, targetY).type;
            if (type != Tile::TileType::Wall && type != Tile::TileType::Void) {
                m_possessedEntity->setX(targetX);
                m_possessedEntity->setY(targetY);
                m_moveCooldown = MOVE_COOLDOWN_TIME;
            }
        }
    }
}

} // namespace emberborn
