#pragma once

#include "Tile.h"
#include <vector>

namespace emberborn {

class TileMap {
public:
  TileMap(int width, int height);
  ~TileMap();

  int getWidth() const noexcept { return m_width; }
  int getHeight() const noexcept { return m_height; }

  bool inBounds(int x, int y) const noexcept {
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
  }

  const Tile& getTile(int x, int y) const;
  void setTile(int x, int y, Tile::TileType type);

private:
  int m_width;
  int m_height;
  std::vector<Tile> m_tiles;
};

} // namespace emberborn
