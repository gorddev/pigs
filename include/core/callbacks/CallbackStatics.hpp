#pragma once
#include "AppStatus.hpp"
#include "SDL_ForwardDeclaration.hpp"

/* Created by Gordie Novak on 5/29/26.
 * Purpose: 
 * Contains some of the static functions the user can use for callback purposes. */

class Application;

namespace pg {

    class Engine;

    struct CallbackStatics {
    private:
        inline static AppStatus (*loopFunction)(Engine&, Application*) = nullptr;
        inline static AppStatus (*eventFunction)(Engine&, Application*, const SDL_Event&) = nullptr;
        inline static void (*quitFunction)(Engine&, Application*) = nullptr;

        friend void setLoopCallback(AppStatus (*loop)(Engine&, Application*));
        friend void setEventCallback(AppStatus (*onEvent)(Engine&, Application*, const SDL_Event&));
        friend void setQuitCallback(void (*onQuit)(Engine&, Application*));
        friend SDL_AppResult (::SDL_AppInit(void**, int, char**));
        friend SDL_AppResult (::SDL_AppIterate(void*));
        friend SDL_AppResult (::SDL_AppEvent(void*, SDL_Event*));
        friend void (::SDL_AppQuit(void*, SDL_AppResult));
    };

    inline void setLoopCallback(AppStatus (*loop)(Engine&, Application*)) {
        CallbackStatics::loopFunction = loop;
    }

    inline void setEventCallback(AppStatus (*onEvent)(Engine&, Application*, const SDL_Event&)) {
        CallbackStatics::eventFunction = onEvent;
    }

    inline void setQuitCallback(void (*onQuit)(Engine&, Application*)) {
        CallbackStatics::quitFunction = onQuit;
    }
}