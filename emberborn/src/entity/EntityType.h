#pragma once
#include <cstdint>

namespace emberborn {

enum class EntityType : uint8_t {
    Player = 0,
    Skeleton,
    Orc,
    BossGillBates,
    Count
};

} // namespace emberborn
