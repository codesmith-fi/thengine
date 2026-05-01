#pragma once

#include "thengine/Entity.h"
#include "thengine/primitives/Color.h"
#include <memory>

namespace thengine {

class Texture;

class Sprite : public Entity {
public:
    Sprite();
    ~Sprite() override;

    void render(Renderer& renderer) override;

    Color m_color;
    std::shared_ptr<Texture> m_texture;
};

} // namespace thengine
