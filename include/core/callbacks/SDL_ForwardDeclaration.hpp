#pragma once
#include <SDL3/SDL_init.h>

/* Created by Gordie Novak on 5/29/26.
 * Purpose: 
 * just contains forward declarations for SDL callback functions */

extern "C" SDL_AppResult SDL_AppInit(void**, int, char**);
extern "C" SDL_AppResult SDL_AppIterate(void*);
extern "C" SDL_AppResult SDL_AppEvent(void*, SDL_Event*);
extern "C" void SDL_AppQuit(void*, SDL_AppResult);
