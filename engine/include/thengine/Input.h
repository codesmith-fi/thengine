#pragma once

namespace thengine {

enum class Key {
    Escape,
    W,
    A,
    S,
    D,
    Space
};

class Input {
public:
    [[nodiscard]] static bool isKeyPressed(Key key) noexcept;
};

} // namespace thengine
