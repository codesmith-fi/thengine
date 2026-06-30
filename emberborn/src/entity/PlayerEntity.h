#pragma once
#include "Entity.h"

namespace emberborn {

class PlayerEntity : public Entity {
public:
    PlayerEntity();
    ~PlayerEntity() override = default;
};

} // namespace emberborn
