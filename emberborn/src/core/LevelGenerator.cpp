#include "LevelGenerator.h"
#include <random>
#include <algorithm>

namespace emberborn {

void LevelGenerator::generateBasicLayout(TileMap& tileMap, int minRoomSize, int maxRoomSize, int maxRooms) {
	int mapWidth = tileMap.getWidth();
	int mapHeight = tileMap.getHeight();

	// 1. Initialize entire map with Void tiles (instead of Walls)
	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			tileMap.setTile(x, y, Tile::TileType::Void);
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
	int roomsCarved = 0;

	// 3. Iterative room carving loop
	for (int i = 0; i < maxRooms; ++i) {
		int roomW = 0;
		int roomH = 0;
		int roomX = 0;
		int roomY = 0;
		bool foundValidSpace = false;

		// Retry up to 100 times to find a non-overlapping spot
		for (int attempt = 0; attempt < 100; ++attempt) {
			roomW = widthDist(gen);
			roomH = heightDist(gen);

			std::uniform_int_distribution<int> xDist(1, mapWidth - roomW - 2);
			std::uniform_int_distribution<int> yDist(1, mapHeight - roomH - 2);

			roomX = xDist(gen);
			roomY = yDist(gen);

			// Check area with 1-tile padding boundary (x - 1 to x + width and y - 1 to y + height)
			bool hasOverlap = false;
			int startCheckX = std::max(0, roomX - 1);
			int endCheckX = std::min(mapWidth - 1, roomX + roomW);
			int startCheckY = std::max(0, roomY - 1);
			int endCheckY = std::min(mapHeight - 1, roomY + roomH);

			for (int checkY = startCheckY; checkY <= endCheckY; ++checkY) {
				for (int checkX = startCheckX; checkX <= endCheckX; ++checkX) {
					if (tileMap.getTile(checkX, checkY).type == Tile::TileType::Floor) {
						hasOverlap = true;
						break;
					}
				}
				if (hasOverlap) {
					break;
				}
			}

			if (!hasOverlap) {
				foundValidSpace = true;
				break;
			}
		}

		// If no valid space was found, skip carving this room
		if (!foundValidSpace) {
			continue;
		}

		int currentCenterX = roomX + roomW / 2;
		int currentCenterY = roomY + roomH / 2;

		// 4. Carve current room to Floor and surround it with Walls (where Void)
		for (int y = roomY - 1; y <= roomY + roomH; ++y) {
			for (int x = roomX - 1; x <= roomX + roomW; ++x) {
				if (tileMap.inBounds(x, y)) {
					// Inside the room boundaries -> carve Floor
					if (x >= roomX && x < roomX + roomW && y >= roomY && y < roomY + roomH) {
						tileMap.setTile(x, y, Tile::TileType::Floor);
					} else {
						// Outer boundary -> set to Wall only if currently Void (never overwrite existing Floor)
						if (tileMap.getTile(x, y).type == Tile::TileType::Void) {
							tileMap.setTile(x, y, Tile::TileType::Wall);
						}
					}
				}
			}
		}

		// 5. Connect room to the previous one with L-shaped corridor if it's not the first one
		if (roomsCarved > 0) {
			// Horizontal corridor step: travel from currentCenterX to prevCenterX at currentCenterY
			int startX = std::min(currentCenterX, prevCenterX);
			int endX = std::max(currentCenterX, prevCenterX);
			for (int x = startX; x <= endX; ++x) {
				tileMap.setTile(x, currentCenterY, Tile::TileType::Floor);

				// Add wall borders above and below the horizontal path if they are Void
				for (int dy : {-1, 1}) {
					int ny = currentCenterY + dy;
					if (tileMap.inBounds(x, ny)) {
						if (tileMap.getTile(x, ny).type == Tile::TileType::Void) {
							tileMap.setTile(x, ny, Tile::TileType::Wall);
						}
					}
				}
			}

			// Vertical corridor step: travel from currentCenterY to prevCenterY at prevCenterX
			int startY = std::min(currentCenterY, prevCenterY);
			int endY = std::max(currentCenterY, prevCenterY);
			for (int y = startY; y <= endY; ++y) {
				tileMap.setTile(prevCenterX, y, Tile::TileType::Floor);

				// Add wall borders to the left and right of the vertical path if they are Void
				for (int dx : {-1, 1}) {
					int nx = prevCenterX + dx;
					if (tileMap.inBounds(nx, y)) {
						if (tileMap.getTile(nx, y).type == Tile::TileType::Void) {
							tileMap.setTile(nx, y, Tile::TileType::Wall);
						}
					}
				}
			}

			// Add wall borders at the 4 diagonals of the turn/intersection corner (prevCenterX, currentCenterY)
			for (int dx : {-1, 1}) {
				for (int dy : {-1, 1}) {
					int nx = prevCenterX + dx;
					int ny = currentCenterY + dy;
					if (tileMap.inBounds(nx, ny)) {
						if (tileMap.getTile(nx, ny).type == Tile::TileType::Void) {
							tileMap.setTile(nx, ny, Tile::TileType::Wall);
						}
					}
				}
			}
		}

		// Update previous centers and count of carved rooms
		prevCenterX = currentCenterX;
		prevCenterY = currentCenterY;
		roomsCarved++;
	}
}

} // namespace emberborn
