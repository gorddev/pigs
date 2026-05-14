#include "Mouse.hpp"

namespace pig {
    Mouse::Mouse() { updateWithSDL(); }

    void Mouse::updateWithSDL() {
        // Now get the current mouse positions!
        SDL_GetRelativeMouseState(&deltaMousePos.x, &deltaMousePos.y);
        // get all the juicy mouse flags.
        mouseFlags = SDL_GetMouseState(&mousePos.x, &mousePos.y);
        // that's it
    }

    void Mouse::onSDLMouseMotion(SDL_Event& e) {
        mousePos = {e.motion.x, e.motion.y};
        deltaMousePos = {e.motion.xrel, e.motion.yrel};
    }

    void Mouse::onSDLMouseButtonDown(SDL_Event& e) {
        mouseFlags |= e.button.button;
    }

    void Mouse::onSDLMouseButtonUp(SDL_Event& e) {
        mouseFlags &= ~e.button.button;
    }

    void Mouse::onSDLMouseWheel(SDL_Event& e) {
        deltaScrollWheel = {e.wheel.x, e.wheel.y};
    }

    void Mouse::clearMouseWheel() {
        deltaScrollWheel = {0, 0};
    }
}
