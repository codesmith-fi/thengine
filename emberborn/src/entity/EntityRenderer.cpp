#include "EntityRenderer.h"
#include "Entity.h"
#include "thengine/graphics/SpriteBatch.h"
#include "thengine/graphics/Texture.h"

namespace emberborn {

void EntityRenderer::registerTexture(EntityType type, std::shared_ptr<thengine::Texture> texture) {
    size_t idx = static_cast<size_t>(type);
    if (idx < m_textures.size()) {
        m_textures[idx] = texture;
    }
}

void EntityRenderer::render(thengine::SpriteBatch& spriteBatch, const std::vector<std::shared_ptr<Entity>>& entities, float tileSize) const {
    for (const auto& entity : entities) {
        if (!entity) continue;

        size_t idx = static_cast<size_t>(entity->getType());
        if (idx >= m_textures.size()) continue;

        const auto& texture = m_textures[idx];
        if (!texture) continue;

        // Calculate world coordinates
        thengine::Vector2 position(
            static_cast<float>(entity->getX()) * tileSize,
            static_cast<float>(entity->getY()) * tileSize
        );

        // Scale factors to fit tileSize
        thengine::Vector2 scale(
            tileSize / static_cast<float>(texture->getWidth()),
            tileSize / static_cast<float>(texture->getHeight())
        );

        // Render depth set to 0.2f (above tiles at 0.1f)
        spriteBatch.draw(
            texture,
            position,
            scale,
            0.0f,                   // rotation
            {0.0f, 0.0f},           // origin (top-left)
            {255, 255, 255, 255},   // white tint
            0.2f                    // depth
        );
    }
}

} // namespace emberborn
