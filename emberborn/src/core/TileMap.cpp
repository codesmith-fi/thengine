#include "TileMap.h"
#include "Constants.h"
#include <algorithm>
#include <cmath>

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

void TileMap::setExplored(int x, int y, bool explored) {
  if (inBounds(x, y)) {
    m_tiles[y * m_width + x].isExplored = explored;
  }
}

std::vector<thengine::LineSegment> TileMap::getObstacleEdges(const thengine::Vector2& center, float radius) const {
  std::vector<thengine::LineSegment> edges;

  // Convert pixel bounds to grid coordinates
  int minX = static_cast<int>(std::floor((center.x - radius) / TILE_SIZE));
  int maxX = static_cast<int>(std::floor((center.x + radius) / TILE_SIZE));
  int minY = static_cast<int>(std::floor((center.y - radius) / TILE_SIZE));
  int maxY = static_cast<int>(std::floor((center.y + radius) / TILE_SIZE));

  // Clamp bounds to map size
  minX = std::max(0, minX);
  maxX = std::min(m_width - 1, maxX);
  minY = std::max(0, minY);
  maxY = std::min(m_height - 1, maxY);

  for (int y = minY; y <= maxY; ++y) {
    for (int x = minX; x <= maxX; ++x) {
      Tile::TileType type = getTile(x, y).type;
      if (type == Tile::TileType::Wall || type == Tile::TileType::Void) {
        float tx = static_cast<float>(x) * TILE_SIZE;
        float ty = static_cast<float>(y) * TILE_SIZE;

        // Vertices of this tile
        thengine::Vector2 tl(tx, ty);
        thengine::Vector2 tr(tx + TILE_SIZE, ty);
        thengine::Vector2 bl(tx, ty + TILE_SIZE);
        thengine::Vector2 br(tx + TILE_SIZE, ty + TILE_SIZE);

        // Add top edge if neighbor is not wall/void
        if (y == 0 || (getTile(x, y - 1).type != Tile::TileType::Wall && getTile(x, y - 1).type != Tile::TileType::Void)) {
          edges.push_back({tl, tr});
        }
        // Add bottom edge
        if (y == m_height - 1 || (getTile(x, y + 1).type != Tile::TileType::Wall && getTile(x, y + 1).type != Tile::TileType::Void)) {
          edges.push_back({bl, br});
        }
        // Add left edge
        if (x == 0 || (getTile(x - 1, y).type != Tile::TileType::Wall && getTile(x - 1, y).type != Tile::TileType::Void)) {
          edges.push_back({tl, bl});
        }
        // Add right edge
        if (x == m_width - 1 || (getTile(x + 1, y).type != Tile::TileType::Wall && getTile(x + 1, y).type != Tile::TileType::Void)) {
          edges.push_back({tr, br});
        }
      }
    }
  }

  return edges;
}

} // namespace emberborn
