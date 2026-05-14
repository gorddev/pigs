cmake_minimum_required(VERSION 3.29)
project(pigs)

message(STATUS "DEBUG: C_COMPILER is ${CMAKE_C_COMPILER}")
message(STATUS "DEBUG: CXX_COMPILER is ${CMAKE_CXX_COMPILER}")

set(CMAKE_CXX_STANDARD 23)

option(SDL_Render_OpenGL_Core "Whether Embedded Systems lib is enabled" ON)

if (EMSCRIPTEN)
    set(SDL_Render_OpenGL_Core OFF)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# enables/disables open gl
if (SDL_Render_OpenGL_Core)
    message("pigs: Standard OpenGL 4.1 enabled. OpenGL_ES set to OFF.")
else()
    message("pigs: OpenGL_ES 3.0 enabled. Open_GL Core 4.1 disabled.")
endif()

# we have to include fetch content to we get repos from git
include(FetchContent)

# requires SDL3
find_package(SDL3 REQUIRED)

####################
# FetchContent Below
####################

if(NOT SDL3_FOUND AND NOT EMSCRIPTEN)
    message("Installing and loading SDL3")
    # ..........................
    #############################
    # SDL 3
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.x
    )
    FetchContent_MakeAvailable(SDL3)
    #############################
endif()

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
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
)

target_link_libraries(ImGui PRIVATE
    SDL3::SDL3
)

# End ImGui compilation

#########################
# Glad Compilation
#########################

if(SDL_Render_OpenGL_Core)
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


#########################
# Main compilation
#########################
add_library(pigs
    src/core/Window.cpp
    src/core/Clock.cpp
    src/core/Mouse.cpp
    src/toolkit/filesystem/filesystem.cpp
    src/toolkit/filesystem/path.cpp
    src/toolkit/OpenGL/Texture.cpp
    src/toolkit/types/vec.cpp
    src/toolkit/OpenGL/glShaders.cpp
)

target_precompile_headers(pigs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pigs.h
)

target_include_directories(pigs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)

target_include_directories(pigs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core
)

target_link_libraries(pigs PUBLIC
    ImGui
    GLAD
    SDL3::SDL3
)

target_compile_options(pigs PRIVATE
    -O3
    -fno-rtti
    -flto
)

target_link_options(pigs PRIVATE
    -flto
)


if(SDL_Render_OpenGL_Core)
    message("Core Core core")
    target_compile_options(pigs PUBLIC
        -DPIGS_OpenGL_Core
    )
else()
    target_compile_options(pigs PUBLIC
        -DPIGS_OpenGL_ES
    )
endif()


if(PIGS_OpenGL_ES)
    message("pigs: Including glad4 directories")
    target_include_directories(pigs PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad4
    )
else()
    message("pigs: Including glades directories")
    target_include_directories(pigs PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad_es
    )
endif ()
