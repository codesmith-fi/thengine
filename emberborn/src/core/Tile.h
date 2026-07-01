#pragma once

namespace emberborn {

struct Tile {
  enum class TileType {
    Void = 0,
    Floor,
    Wall,
    Water
  };

  TileType type = TileType::Void;
  bool isExplored = false;

  // Helper functions
  bool isWalkable() const {
    return type == TileType::Floor;
  }

  bool isBlocksLight() const {
    return type == TileType::Wall;
  }
};

} // namespace emberborn
