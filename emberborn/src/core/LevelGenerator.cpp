#include "LevelGenerator.h"
#include <random>
#include <algorithm>

namespace emberborn {

void LevelGenerator::generateBasicLayout(TileMap& tileMap, int minRoomSize, int maxRoomSize, int maxRooms) {
	int mapWidth = tileMap.getWidth();
	int mapHeight = tileMap.getHeight();

	// 1. Initialize entire map with Wall tiles
	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			tileMap.setTile(x, y, Tile::TileType::Wall);
		}
	}

	// Safety clamps
	minRoomSize = std::max(2, minRoomSize);
	maxRoomSize = std::max(minRoomSize, maxRoomSize);
	maxRooms = std::max(1, maxRooms);

	int maxPossibleWidth = std::max(2, mapWidth - 3);
	int maxPossibleHeight = std::max(2, mapHeight - 3);
	minRoomSize = std::min(minRoomSize, maxPossibleWidth);
	maxRoomSize = std::min(maxRoomSize, maxPossibleWidth);
	int minRoomSizeH = std::min(minRoomSize, maxPossibleHeight);
	int maxRoomSizeH = std::min(maxRoomSize, maxPossibleHeight);

	// 2. Setup RNG
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> widthDist(minRoomSize, maxRoomSize);
	std::uniform_int_distribution<int> heightDist(minRoomSizeH, maxRoomSizeH);

	int prevCenterX = 0;
	int prevCenterY = 0;

	// 3. Iterative room carving loop
	for (int i = 0; i < maxRooms; ++i) {
		int roomW = widthDist(gen);
		int roomH = heightDist(gen);

		std::uniform_int_distribution<int> xDist(1, mapWidth - roomW - 2);
		std::uniform_int_distribution<int> yDist(1, mapHeight - roomH - 2);

		int roomX = xDist(gen);
		int roomY = yDist(gen);

		int currentCenterX = roomX + roomW / 2;
		int currentCenterY = roomY + roomH / 2;

		// Carve current room to Floor
		for (int y = roomY; y < roomY + roomH; ++y) {
			for (int x = roomX; x < roomX + roomW; ++x) {
				tileMap.setTile(x, y, Tile::TileType::Floor);
			}
		}

		// If this is not the first room, connect it to the previous room
		if (i > 0) {
			// Horizontal corridor: travel from currentCenterX to prevCenterX at currentCenterY
			int startX = std::min(currentCenterX, prevCenterX);
			int endX = std::max(currentCenterX, prevCenterX);
			for (int x = startX; x <= endX; ++x) {
				tileMap.setTile(x, currentCenterY, Tile::TileType::Floor);
			}

			// Vertical corridor: travel from currentCenterY to prevCenterY at prevCenterX
			int startY = std::min(currentCenterY, prevCenterY);
			int endY = std::max(currentCenterY, prevCenterY);
			for (int y = startY; y <= endY; ++y) {
				tileMap.setTile(prevCenterX, y, Tile::TileType::Floor);
			}
		}

		// Update previous centers for the next iteration
		prevCenterX = currentCenterX;
		prevCenterY = currentCenterY;
	}
}

} // namespace emberborn
