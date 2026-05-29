#pragma once

#include "Clock.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Window.hpp"
#include "Scene.hpp"

#include <core/callbacks/SDL_ForwardDeclaration.hpp>

#include "rendering/textures/TextureRegister.hpp"

namespace pg {

    class Engine {
    public:
        Window          window;
        Clock           clock;
        Keyboard        keyboard;
        Mouse           mouse;
        TextureRegister textures{};

        Engine() = default;

    private:
        friend SDL_AppResult (::SDL_AppInit(void**, int, char**));
        friend SDL_AppResult (::SDL_AppIterate(void*));
        friend SDL_AppResult (::SDL_AppEvent(void*, SDL_Event*));

        explicit Engine(int)
            : window(Window::make("Pigs", {300, 300})), textures{} {

            textures.init();
            stbi_set_flip_vertically_on_load(true);
        }

        Engine(Engine&& o) noexcept = default;
        Engine& operator=(Engine&& o) noexcept = default;

        void on_start_loop() {
            glClear(GL_COLOR_BUFFER_BIT); //< Clear the canvas.

            clock.tick(); //< Increment the clock.

            // Copy data from SDL into the keyboard struct.
            const auto ptr = SDL_GetKeyboardState(nullptr);
            std::memcpy(keyboard.keys.data(), ptr, Keyboard::maxKeys);
        }

        void on_end_loop() {
            mouse.clearMouseWheel(); //< Clear current status of the mouse wheel.

            SDL_GL_SwapWindow(window); //< Swap the window to the user.
        }
    };

}
