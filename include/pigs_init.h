#pragma once

#include <core/Engine.hpp>
#include <core/callbacks/CallbackStatics.hpp>

class Application;

/** Initialization function the dev must use to start the engine. You must:
 * - Define @code init@endcode and match the declaration of the function: @code Application* init(pg::Engine&)@endcode
 * - Set a loop callback with @code pg::setLoopCallback(func)@endcode. The loop callback must match the
 * signature: @code AppStatus loopFunc(Engine&, Application*)@endcode
 * - Return @code nullptr@endcode if you do not wish to define the @code Application@endcode object ...
 * - ... or return a valid pointer to an @code Application@endcode object that you instantiate.
 *
 * Additionally, you can define callback functions for events and application exit with the functions:
 * - @code pg::setEventCallback(AppStatus (*onEvent)(Engine&, Application*, const SDL_Event&))@endcode
 * - @code pg::setQuitCallback(AppStatus (*onQuit)(Engine&, Application*)@endcode
 */
Application* init(pg::Engine&);