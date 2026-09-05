/* Created by Gordie Novak on 5/29/26.
 * Purpose:
 * Contains all callback functions */

#include "SDL3/SDL_init.h"
#include <pigs_init.h>

#define SDL_MAIN_USE_CALLBACKS 1
// ReSharper disable once CppUnusedIncludeDirective
#include <SDL3/SDL_main.h>

#define GLM_ENABLE_EXPERIMENTAL

alignas(64) inline pg::Engine engine;

SDL_AppResult SDL_AppInit(void**, int argc, char* argv[]) {
    // First initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    // Create the engine.
    engine.init(argc, argv);

    // -----------------------------
    // Call the user's init function
    // -----------------------------
    pg::AppStatus app_status = init(engine);

    if (engine.config.dev.no_loop) {
        std::puts("\x1B[36m\x1B[3mdev: \"no_loop\" specified in config.dev.no_loop. exiting\x1B[0m");
        return SDL_APP_SUCCESS;
    } else if (app_status == pg::AppStatus::APP_QUIT)
      return SDL_APP_SUCCESS;
    if (!engine.config.callback.loop) {
        PG_Panic("The dev failed to set a looping function. Thus, no program will run.\n"
                "Please setup a loop function with engine.setLoop(...).\n"
                "\tloop signature: pg::AppStatus loop(pg::Engine&);");
    }
    PG_CWarn_(init.no_crash_func, !engine.config.callback.crash,
        "No crash function set. Please set one with engine.onCrash(...) {Functor Signature: void(Engine&)}");
    PG_CWarn_(init.no_event_func, !engine.config.callback.event,
        "No event function set. Please set one with engine.onEvent(...) {Functor Signature: AppStatus(Engine&, const SDL_Event&)}");
    PG_CWarn_(init.no_quit_func, !engine.config.callback.quit,
        "No quit function set. Please set one with engine.onQuit(...)  {Functor Signature: void(Engine&)}");

    return static_cast<SDL_AppResult>(app_status);
}

SDL_AppResult SDL_AppIterate(void*) {

    engine.on_start_loop();

    pg::AppStatus result = engine.config.callback.loop.func(engine);

    engine.on_end_loop();

    return static_cast<SDL_AppResult>(result);
}


SDL_AppResult SDL_AppEvent(void*, SDL_Event* event) {

    switch (event->type) {
    case SDL_EVENT_QUIT:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
    case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
    case SDL_EVENT_WINDOW_MINIMIZED:
    case SDL_EVENT_WINDOW_MAXIMIZED:
        engine.window.on_SDLWindowEvent(*event);
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        engine.mouse.onSDLMouseButtonDown(*event);
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        engine.mouse.onSDLMouseButtonUp(*event);
        break;
    case SDL_EVENT_MOUSE_MOTION:
        engine.mouse.onSDLMouseMotion(*event);
        break;
    case SDL_EVENT_MOUSE_WHEEL:
        engine.mouse.onSDLMouseWheel(*event);
        break;
    case SDL_EVENT_KEY_DOWN:
        engine.keyboard.onKeyPress(*event);
    default:
        break;
    }

    if (engine.config.callback.event) {
        auto result = engine.config.callback.event.func(engine, *event);
        return static_cast<SDL_AppResult>(result);
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult) {
    if (engine.config.callback.quit) {
        engine.config.callback.quit.func(engine);
    }
    SDL_Quit();
}
