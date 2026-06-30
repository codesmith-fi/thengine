#pragma once

#include "core/Tile.h"
#include "core/TileMap.h"
#include "thengine/graphics/Texture.h"
#include "thengine/graphics/SpriteBatch.h"
#include "thengine/view/Camera2D.h"
#include <memory>
#include <vector>
#include <unordered_map>

namespace emberborn {

class TileRenderer {
public:
	TileRenderer() = default;
	~TileRenderer() = default;

	// Disable copy/assignment to prevent accidental resource duplication overhead
	TileRenderer(const TileRenderer&) = delete;
	TileRenderer& operator=(const TileRenderer&) = delete;
	TileRenderer(TileRenderer&&) noexcept = default;
	TileRenderer& operator=(TileRenderer&&) noexcept = default;

	/**
	 * Registers a texture asset for a specific tile type.
	 * Registering a type multiple times automatically registers it as a variation.
	 */
	void registerTexture(Tile::TileType type, std::shared_ptr<thengine::Texture> texture);

	/**
	 * Renders the visible region of the TileMap inside the camera's viewport.
	 * Performs light-weight frustum culling mathematically without calling low-level platform APIs.
	 */
	void render(thengine::SpriteBatch& spriteBatch, const TileMap& tileMap, const thengine::Camera2D& camera, float screenWidth, float screenHeight, float tileSize = 64.0f) const;

private:
	// Internal storage mapping tile types to one or more texture variants
	std::unordered_map<Tile::TileType, std::vector<std::shared_ptr<thengine::Texture>>> m_textures;
};

} // namespace emberborn
