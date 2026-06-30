#include "TileRenderer.h"
#include <algorithm>
#include <cmath>

namespace emberborn {

void TileRenderer::registerTexture(Tile::TileType type, std::shared_ptr<thengine::Texture> texture) {
	if (texture) {
		m_textures[type].push_back(texture);
	}
}

void TileRenderer::render(thengine::SpriteBatch& spriteBatch, const TileMap& tileMap, const thengine::Camera2D& camera, float screenWidth, float screenHeight, float tileSize) const {
	float zoom = camera.getZoom();
	if (zoom <= 0.0f) {
		zoom = 1.0f;
	}

	// Calculate visible bounds in world space based on camera position and zoom
	float halfWidth = (screenWidth / zoom) * 0.5f;
	float halfHeight = (screenHeight / zoom) * 0.5f;

	thengine::Vector2 camPos = camera.getPosition();
	float minWorldX = camPos.x - halfWidth;
	float maxWorldX = camPos.x + halfWidth;
	float minWorldY = camPos.y - halfHeight;
	float maxWorldY = camPos.y + halfHeight;

	// Convert world space coordinates to tile grid indices
	int startX = std::max(0, static_cast<int>(std::floor(minWorldX / tileSize)));
	int endX = std::min(tileMap.getWidth() - 1, static_cast<int>(std::ceil(maxWorldX / tileSize)));
	int startY = std::max(0, static_cast<int>(std::floor(minWorldY / tileSize)));
	int endY = std::min(tileMap.getHeight() - 1, static_cast<int>(std::ceil(maxWorldY / tileSize)));

	// Render only the visible tiles in the viewport frustum
	for (int y = startY; y <= endY; ++y) {
		for (int x = startX; x <= endX; ++x) {
			Tile::TileType type = tileMap.getTile(x, y).type;

			auto it = m_textures.find(type);
			if (it == m_textures.end() || it->second.empty()) {
				continue;
			}

			const auto& variants = it->second;
			size_t variantCount = variants.size();
			size_t index = 0;
			if (variantCount > 1) {
				index = static_cast<size_t>((x * 73856093) ^ (y * 19349663)) % variantCount;
			}

			const auto& texture = variants[index];
			if (!texture) {
				continue;
			}

			thengine::Vector2 position(x * tileSize, y * tileSize);
			thengine::Vector2 scale(
				tileSize / static_cast<float>(texture->getWidth()),
				tileSize / static_cast<float>(texture->getHeight())
			);

			spriteBatch.draw(
				texture,
				position,
				scale,
				0.0f,					// rotation
				{0.0f, 0.0f},			// origin (top-left)
				{255, 255, 255, 255},	// color (white tint)
				0.1f					// depth (render below entities)
			);
		}
	}
}

} // namespace emberborn
