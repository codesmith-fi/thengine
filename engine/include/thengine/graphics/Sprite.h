#pragma once

#include "thengine/Entity.h"
#include "thengine/primitives/Color.h"

namespace thengine {

class Sprite : public Entity {
public:
    Sprite();
    ~Sprite() override;

    void render(Renderer& renderer) override;

    Color m_color;
};

} // namespace thengine
