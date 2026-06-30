#include "TileMap.h"

namespace emberborn {

TileMap::TileMap(int width, int height)
    : m_width(width), m_height(height) {
  if (m_width < 0) m_width = 0;
  if (m_height < 0) m_height = 0;
  m_tiles.resize(m_width * m_height);
}

TileMap::~TileMap() = default;

const Tile& TileMap::getTile(int x, int y) const {
  static const Tile voidTile = { Tile::TileType::Void };
  if (!inBounds(x, y)) {
    return voidTile;
  }
  return m_tiles[y * m_width + x];
}

void TileMap::setTile(int x, int y, Tile::TileType type) {
  if (inBounds(x, y)) {
    m_tiles[y * m_width + x].type = type;
  }
}

} // namespace emberborn
