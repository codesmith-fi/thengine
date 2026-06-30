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

	// Calculate visible bounds in world space, accounting for camera rotation
	float angle = camera.getRotation();
	float cosA = std::abs(std::cos(angle));
	float sinA = std::abs(std::sin(angle));

	float halfWidth = (screenWidth / zoom) * 0.5f;
	float halfHeight = (screenHeight / zoom) * 0.5f;

	// Calculate the exact AABB (Axis-Aligned Bounding Box) of the rotated viewport
	float extentX = halfWidth * cosA + halfHeight * sinA;
	float extentY = halfWidth * sinA + halfHeight * cosA;

	thengine::Vector2 camPos = camera.getPosition();
	float minWorldX = camPos.x - extentX;
	float maxWorldX = camPos.x + extentX;
	float minWorldY = camPos.y - extentY;
	float maxWorldY = camPos.y + extentY;

	// Convert world space coordinates to tile grid indices
	int startX = std::max(0, static_cast<int>(std::floor(minWorldX / tileSize)));
	int endX = std::min(tileMap.getWidth() - 1, static_cast<int>(std::ceil(maxWorldX / tileSize)));
	int startY = std::max(0, static_cast<int>(std::floor(minWorldY / tileSize)));
	int endY = std::min(tileMap.getHeight() - 1, static_cast<int>(std::ceil(maxWorldY / tileSize)));

	// Pre-fetch and cache render data (textures and scale vectors) for each tile type to avoid map lookups in the loop
	struct TextureRenderData {
		std::shared_ptr<thengine::Texture> texture;
		thengine::Vector2 scale;
	};

	std::vector<TextureRenderData> cachedRenderData[4];
	for (int i = 0; i < 4; ++i) {
		auto type = static_cast<Tile::TileType>(i);
		if (type == Tile::TileType::Void) {
			continue; // Skip Void early
		}

		auto it = m_textures.find(type);
		if (it != m_textures.end()) {
			for (const auto& tex : it->second) {
				if (tex) {
					thengine::Vector2 scale(
						tileSize / static_cast<float>(tex->getWidth()),
						tileSize / static_cast<float>(tex->getHeight())
					);
					cachedRenderData[i].push_back({tex, scale});
				}
			}
		}
	}

	// Render only the visible non-Void tiles in the viewport frustum
	for (int y = startY; y <= endY; ++y) {
		for (int x = startX; x <= endX; ++x) {
			Tile::TileType type = tileMap.getTile(x, y).type;
			if (type == Tile::TileType::Void) {
				continue; // 1. Void Early-Out: Avoid lookup and math for empty tiles
			}

			int typeIdx = static_cast<int>(type);
			const auto& variants = cachedRenderData[typeIdx];
			if (variants.empty()) {
				continue;
			}

			size_t variantCount = variants.size();
			size_t index = 0;
			if (variantCount > 1) {
				index = static_cast<size_t>((x * 73856093) ^ (y * 19349663)) % variantCount;
			}

			const auto& renderData = variants[index];

			spriteBatch.draw(
				renderData.texture,
				thengine::Vector2(x * tileSize, y * tileSize),
				renderData.scale,
				0.0f,					// rotation
				{0.0f, 0.0f},			// origin (top-left)
				{255, 255, 255, 255},	// color (white tint)
				0.1f					// depth (render below entities)
			);
		}
	}
}

} // namespace emberborn
