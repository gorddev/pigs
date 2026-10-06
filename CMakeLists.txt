cmake_minimum_required(VERSION 3.29)

project(pigs)

set(CMAKE_CXX_STANDARD 26)
option(PIGS_OpenGL_Core "Whether Core OpenGL lib is enabled" ON)

###################
# SDL3
###################

set(SDL_AUDIO OFF          CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF         CACHE BOOL "" FORCE)
set(SDL_TESTS OFF          CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF   CACHE BOOL "" FORCE)
set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
set(SDL_GPU OFF            CACHE BOOL "" FORCE)
set(SDL3_DISABLE_UNINSTALL OFF CACHE BOOL "" FORCE)
set(SDL_RENDER OFF         CACHE BOOL "" FORCE)
set(SDL_FILESYSTEM OFF     CACHE BOOL "" FORCE)
set(SDL_FILE OFF           CACHE BOOL "" FORCE)
set(SDL_DIALOG OFF         CACHE BOOL "" FORCE)
set(SDL_ATOMIC OFF         CACHE BOOL "" FORCE)
set(SDL_STATIC ON          CACHE BOOL "" FORCE)
set(SDL_SHARED OFF         CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL3) #]]

####################
# ImGUI below.
####################

add_library(ImGui STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends/imgui_impl_sdl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(ImGui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
)

target_compile_options(ImGui PRIVATE
  -D__STDC_WANT_LIB_EXT1__=1
)

target_link_libraries(ImGui PRIVATE
    SDL3::SDL3-static
)


#########################
# GLAD compilation
#########################

if (EMSCRIPTEN)
  set(PIGS_OpenGL_Core OFF)
  message("Emscripten enabled")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# enables/disables open gl
if (PIGS_OpenGL_Core)
  message("pigs: Standard OpenGL 4.1 enabled. OpenGL_ES set to OFF.")
else()
  message("pigs: OpenGL_ES 3.0 enabled. Open_GL Core 4.1 disabled.")
endif()


if(PIGS_OpenGL_Core)
  add_library(GLAD STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad4/glad/glad.c
    )
  target_include_directories(GLAD PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad4
    )
else()
  add_library(GLAD STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad_es/glad/glad.c
    )
  target_include_directories(GLAD PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad_es
    )
endif()
target_compile_options(GLAD PRIVATE
    -Oz
)

#########################
# STB Image compilation
#########################
add_library(stb_image
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb_image/stb_image.c
)
target_include_directories(stb_image PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb_image
)
target_compile_options(stb_image PRIVATE
    -Oz
)


#########################
# GLM compilation
#########################
#find_package(glm REQUIRED)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glm) #]]

#########################
# Miniaudio compilation
#########################
add_subdirectory(external/miniaudio)

# ...........................

# ...........................

#########################
# Main compilation
#########################
add_library(pigs
    ##### src/core
    src/core/Window.cpp
    src/core/Engine.cpp
    src/core/Clock.cpp
    src/core/Mouse.cpp
    src/core/Config.cpp
    src/core/Audio.cpp

    src/core/rendering/shaders/ShaderManager.cpp
    src/core/rendering/shaders/universal/UniversalUniforms.cpp

    src/core/config/config_Audio.cpp

    src/core/vitals/Exec.cpp

    src/core/callbacks/Callbacks.cpp

    src/core/rendering/textures/TextureRegister.cpp

    src/core/errors/panic.cpp

    src/core/scripting/Tokenizer.cpp

    ##### src/toolkit
    src/core/rendering/OpenGL/Texture.cpp
    src/core/rendering/OpenGL/FrameBuffer.cpp
    src/core/rendering/OpenGL/glShaders.cpp

    src/toolkit/types/vec.cpp

    src/toolkit/platform/executable_dir.cpp
    src/toolkit/platform/memory/memory_usage.cpp
    src/toolkit/platform/memory/format_memory.cpp
)

target_include_directories(pigs PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/core/errors
)

target_include_directories(pigs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)

target_link_libraries(pigs PUBLIC
    ImGui
    GLAD
    SDL3::SDL3-static
    stb_image
    glm::glm
    miniaudio
    stdc++
)
target_compile_options(pigs PRIVATE
    -Werror
    -Wall
    -Wextra
    -Wno-unused-value
    -Wno-parentheses
    -Wno-error=class-memaccess
)

target_compile_options(pigs PUBLIC
    -O0
    -ffast-math
    -freflection
    -fno-exceptions
    -fno-rtti
    -fsanitize=address,undefined
    -fno-omit-frame-pointer
    -Wno-unused-command-line-argument
    -Wno-unknown-warning-option
    -std=c++26
)

target_link_options(pigs PRIVATE
    -ffast-math
    -fno-exceptions
    -fno-rtti
    -std=c++26
)

target_link_options(pigs PUBLIC
-fsanitize=address,undefined
)

if(PIGS_OpenGL_Core)
  message("pigs: Including glad4 directories")
  target_include_directories(pigs PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad4
    )
  target_compile_options(pigs PUBLIC
        -DPIGS_OpenGL_Core
    )
else()
  message("pigs: Including glades directories")
  target_include_directories(pigs PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad_es
    )
  target_compile_options(pigs PUBLIC
        -DPIGS_OpenGL_ES
    )
endif ()


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
