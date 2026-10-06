#pragma once

#include "Clock.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Window.hpp"
#include "Config.hpp"

#include <core/callbacks/SDL_ForwardDeclaration.hpp>

#include "Audio.hpp"
#include "core/rendering/shaders/ShaderManager.hpp"
#include "core/rendering/textures/TextureRegister.hpp"
#include "toolkit/types/void_ptr.hpp"
#include "vitals/Exec.hpp"

namespace pg {

    /**
     * @brief The core engine class for the PIGS framework.
     *
     * The Engine class aggregates essential components such as the window, clock,
     * input handling (keyboard and mouse), and texture management.
     */
    class Engine {
    public:
        Audio           audio;      ///< The audio manager.
        Window          window;     ///< The application window.
        Clock           clock;      ///< The system clock for time-based operations.
        Keyboard        keyboard;   ///< The keyboard input state.
        Mouse           mouse;      ///< The mouse input state.
        TextureRegister textures{}; ///< The global texture register.
        ShaderManager   shaders;    ///< Allows for management of shaders easily.
        Config&         config = opt_nmspc::pg_internal_options;
        Exec            exec;       ///< Information regarding the executable and userdata.
        /**
         * @brief Default constructor for Engine.
         */
        Engine() = default;

        template<typename Func>
        Engine& setLoop(Func loop) {
            config.callback.loop = loop;
            return *this;
        }
        template<typename Func>
        Engine& onEvent(Func loop) {
            config.callback.event = loop;
            return *this;
        }
        template<typename Func>
        Engine& onQuit(Func loop) {
            config.callback.quit = loop;
            return *this;
        }
        template<typename Func>
        Engine& onCrash(Func loop) {
            config.callback.crash = loop;
            return *this;
        }

    private:
        friend SDL_AppResult (::SDL_AppInit(void**, int, char**));
        friend SDL_AppResult (::SDL_AppIterate(void*));
        friend SDL_AppResult (::SDL_AppEvent(void*, SDL_Event*));

        /**
         * @brief Internal constructor used for engine initialization.
         * @param unused Unused parameter to differentiate from default constructor.
         */
        explicit Engine(int);

        Engine(Engine&& o) noexcept = delete;
        Engine& operator=(Engine&& o) noexcept;

        /**
         * @brief Internal method called at the start of each frame loop.
         *
         * Clears the screen, ticks the clock, and updates the keyboard state.
         */
        void on_start_loop();

        /**
         * @brief Internal method called at the end of each frame loop.
         *
         * Clears mouse wheel status and swaps the window buffers.
         */
        void on_end_loop();

        void init(int argc, char* argv[]);
    };

}
