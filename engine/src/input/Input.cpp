#include "thengine/Input.h"
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_scancode.h>

namespace thengine {

bool Input::isKeyPressed(Key key) noexcept {
    SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;

    switch (key) {
        case Key::Escape: scancode = SDL_SCANCODE_ESCAPE; break;
        case Key::W:      scancode = SDL_SCANCODE_W; break;
        case Key::A:      scancode = SDL_SCANCODE_A; break;
        case Key::S:      scancode = SDL_SCANCODE_S; break;
        case Key::D:      scancode = SDL_SCANCODE_D; break;
        case Key::Space:  scancode = SDL_SCANCODE_SPACE; break;
    }

    if (scancode != SDL_SCANCODE_UNKNOWN) {
        const bool* state = SDL_GetKeyboardState(nullptr);
        return state[scancode];
    }

    return false;
}

} // namespace thengine
