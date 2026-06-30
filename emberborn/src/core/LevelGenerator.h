#pragma once

#include "core/TileMap.h"

namespace emberborn {

class LevelGenerator {
public:
	// Prevent instantiation of this static utility class
	LevelGenerator() = delete;
	~LevelGenerator() = delete;

	/**
	 * Procedurally generates a basic layout on the given TileMap.
	 * Fills the map with Wall tiles first, then carves out two random rooms
	 * and connects them with an L-shaped corridor.
	 */
	static void generateBasicLayout(TileMap& tileMap, int minRoomSize, int maxRoomSize, int maxRooms);
};

} // namespace emberborn
