#pragma once

/* Created by Gordie Novak on 3/16/26.
 * Purpose:
 */


/* OPEN GL Setup */
#ifdef PIGS_OpenGL_Core // If we're using OpenGL 4.1 core
#include "../external/glad4/glad/glad.h"
#define GAN_gladLoadGL gladLoadGLLoader
constexpr char glVersionHeader[] = "#version 410 core\n";

#else                      // If we're using OpenGL ES 3.0
#include "../external/glad_es/glad/glad.h"
#define GAN_gladLoadGL gladLoadGLES2Loader
constexpr char glVersionHeader[] = "#version 300 es\n";
#endif


/* SDL3 Setup */
#include <SDL3/SDL.h>
namespace pig {
    /** Ensure that SDL is initialized only once */
    inline void ensure_SDL_init() {
        [[maybe_unused]] static bool b = []() {
            SDL_Init(SDL_INIT_VIDEO);
            SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");
            return true;
        }();

    }

    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using f32 = float;
    using f64 = double;
}
