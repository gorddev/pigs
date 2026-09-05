#pragma once
#include <SDL3/SDL.h>
#include <toolkit/intdef.h>


/* OPEN GL Setup */
#ifdef PIGS_OpenGL_Core // If we're using OpenGL 4.1 core
#ifndef __gl_h_
#include <glad/glad.h>
#endif
#define PIG_gladLoadGL gladLoadGLLoader
namespace pg {
    constexpr char glVersionHeader[] = "#version 410 core\n";
}

#else                      // If we're using OpenGL ES 3.0
#include "../external/glad_es/glad/glad.h"
#define PIG_gladLoadGL gladLoadGLES2Loader
namespace pg {
    constexpr char glVersionHeader[] = "#version 300 es\n";
}
#endif


#define GL_CHECK() \
do { \
GLenum err = glGetError(); \
if (err != GL_NO_ERROR) \
std::cerr << "GL error: " << err << " at " << __LINE__ << " in " << __FILE_NAME__ << std::endl; \
} while (0)

/* SDL3 Setup */
namespace pg {
    /** Ensure that SDL is initialized only once */
    inline void ensure_SDL_init() {
        [[maybe_unused]] static bool b = []() {
            SDL_Init(SDL_INIT_VIDEO);
            return true;
        }();
    }
}
