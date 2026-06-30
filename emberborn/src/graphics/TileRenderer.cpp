#include "TileRenderer.h"

namespace emberborn {

void TileRenderer::registerTexture(Tile::TileType type, std::shared_ptr<thengine::Texture> texture) {
	if (texture) {
		m_textures[type].push_back(texture);
	}
}

void TileRenderer::render(thengine::SpriteBatch& spriteBatch, const TileMap& tileMap, float tileSize) const {
    int mapWidth = tileMap.getWidth();
    int mapHeight = tileMap.getHeight();

    // Loop through the entire map linearly in memory (Y-then-X for cache friendliness)
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            Tile::TileType type = tileMap.getTile(x, y).type;

            // Fast exit if tile type has no textures registered
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

            // Calculate position directly in world space
            thengine::Vector2 position(x * tileSize, y * tileSize);
            thengine::Vector2 scale(
                tileSize / static_cast<float>(texture->getWidth()),
                tileSize / static_cast<float>(texture->getHeight())
            );

            // Let the SpriteBatch handle culling and Camera matrix transformations automatically!
            spriteBatch.draw(
                texture,
                position,
                scale,
                0.0f,                   // rotation
                {0.0f, 0.0f},           // origin (top-left)
                {255, 255, 255, 255},   // color (white tint)
                0.1f                    // depth (render below entities)
            );
        }
    }
}

} // namespace emberborn
