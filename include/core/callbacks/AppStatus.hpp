#pragma once
#include <SDL3/SDL_init.h>

/** Simple stand-in enum struct for SDL_APP Enumerations */
enum AppStatus {
    APP_CONTINUE    = SDL_APP_CONTINUE,
    APP_QUIT        = SDL_APP_SUCCESS,
    ERROR           = SDL_APP_FAILURE
};
