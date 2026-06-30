#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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

    void registerTexture(const std::string& entityTypeKey, std::shared_ptr<thengine::Texture> texture);

    void render(thengine::SpriteBatch& spriteBatch, const std::vector<std::shared_ptr<Entity>>& entities, float tileSize) const;

private:
    std::unordered_map<std::string, std::shared_ptr<thengine::Texture>> m_textures;
};

} // namespace emberborn
