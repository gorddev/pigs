/* Created by Gordie Novak on 5/29/26.
 * Purpose: 
 * Contains all callback functions */

#include <pigs_init.h>

#include <core/callbacks/CallbackStatics.hpp>

#define SDL_MAIN_USE_CALLBACKS 1
// ReSharper disable once CppUnusedIncludeDirective
#include <SDL3/SDL_main.h>

#define GLM_ENABLE_EXPERIMENTAL

alignas(64) inline pg::Engine engine;
inline Application* app;

SDL_AppResult SDL_AppInit(void**, int, char**) {
    // First initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    // Create the engine.
    engine = pg::Engine(0);

    // Call the user's init function
    try {
        app = init(engine);
    } catch (const std::exception& e) {
        std::cout << "\n!! Uncaught Exception! That's not good !!\n" << e.what() << std::flush;
        return SDL_APP_FAILURE;
    }

    if (!pg::CallbackStatics::loopFunction) {
        pg::panic("pg::internal_init()", "The dev failed to set a looping function. Thus, no program will run.\n"
                                              "Please setup a loop function with pg::setLoopCallback(...).\n");
    }

    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void*) {

    engine.on_start_loop();

    AppStatus result;
    try {
        result = pg::CallbackStatics::loopFunction(engine, app);
    } catch (...) {
        std::cerr << "Uncaught exception called in developer code. Exiting.\n" << std::endl;
        return SDL_APP_FAILURE;
    }

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
    default:
        break;
    }

    if (pg::CallbackStatics::eventFunction) {
        auto result = pg::CallbackStatics::eventFunction(engine, app, *event);
        return static_cast<SDL_AppResult>(result);
    }
    return SDL_APP_CONTINUE;
}


void SDL_AppQuit(void*, SDL_AppResult) {
    if (pg::CallbackStatics::quitFunction) {
        pg::CallbackStatics::quitFunction(engine, app);
    }
    SDL_Quit();
}