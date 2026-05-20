#pragma once

namespace thengine {

enum class Key {
    Escape,
    W,
    A,
    S,
    D,
    Space,
    Up,
    Down,
    Left,
    Right,
    Q,
    E
};

class Input {
public:
    [[nodiscard]] static bool isKeyPressed(Key key) noexcept;
};

} // namespace thengine
