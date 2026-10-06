#pragma once
#include <functional>

#include <core/callbacks/AppStatus.hpp>
#include <core/callbacks/SDL_ForwardDeclaration.hpp>

/* Created by Gordie Novak on 8/22/26.
 * Purpose:
 * Allows the user to manually specify callback functions*/

union SDL_Event;

namespace pg { class Engine; [[noreturn]] void panic();}


namespace pg::config {

    struct Callbacks;

    /** Allows for exclusive setting of functions for engine-exclusive access to callbacks */
    template<typename Func>
    class cb_func {
        std::function<Func> func;
        friend SDL_AppResult (::SDL_AppInit(void**, int, char**));
        friend SDL_AppResult (::SDL_AppIterate(void*));
        friend SDL_AppResult (::SDL_AppEvent(void*, SDL_Event*));
        friend void          (::SDL_AppQuit(void*, SDL_AppResult));
        friend void          (pg::panic());

        template<typename... Args>
        auto operator()(Args...args) {
            return func(std::forward<Args>(args)...);
        }
    public:
        cb_func& operator=(std::function<Func> new_func) {
            func = new_func;
            return *this;
        }
        operator bool() const {
            return bool(func);
        }
    };

    /** Contains each of the primary callbacks the engine will use for:
     * - Looping
     * - Events
     * - Crashing
     * - Quitting the application. **/
    struct Callbacks {
        cb_func<AppStatus(Engine&)> loop;
        cb_func<AppStatus(Engine&, const SDL_Event&)> event;
        cb_func<void(Engine&)> quit;
        cb_func<void(Engine&)> crash;
    };
}
