#pragma once
#include "EntityType.h"
#include <memory>
#include <array>
#include <vector>
#include "thengine/primitives/Color.h"

namespace thengine {
class SpriteBatch;
class Texture;
}

namespace emberborn {

class Entity;

class EntityRenderer {
public:
    EntityRenderer() = default;
    ~EntityRenderer() = default;

    void registerTexture(EntityType type, std::shared_ptr<thengine::Texture> texture);

    void render(thengine::SpriteBatch& spriteBatch, const std::vector<std::shared_ptr<Entity>>& entities, const std::vector<thengine::Color>& tints, float tileSize) const;

private:
    std::array<std::shared_ptr<thengine::Texture>, static_cast<size_t>(EntityType::Count)> m_textures;
};

} // namespace emberborn
