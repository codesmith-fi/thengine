#pragma once
#include "EntityController.h"

namespace emberborn {

class MonsterController : public EntityController {
public:
    MonsterController();
    ~MonsterController() override = default;

    void update(float deltaTime, const TileMap& tileMap) override;
};

} // namespace emberborn
