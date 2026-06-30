#pragma once

#include "Tile.h"
#include "thengine/view/IObstacleProvider.h"
#include <vector>

namespace emberborn {

class TileMap : public thengine::IObstacleProvider {
public:
  TileMap(int width, int height);
  ~TileMap() override;

  int getWidth() const noexcept { return m_width; }
  int getHeight() const noexcept { return m_height; }

  bool inBounds(int x, int y) const noexcept {
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
  }

  const Tile& getTile(int x, int y) const;
  void setTile(int x, int y, Tile::TileType type);

  // Implement IObstacleProvider
  std::vector<thengine::LineSegment> getObstacleEdges(const thengine::Vector2& center, float radius) const override;

private:
  int m_width;
  int m_height;
  std::vector<Tile> m_tiles;
};

} // namespace emberborn
