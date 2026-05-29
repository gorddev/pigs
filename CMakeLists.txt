cmake_minimum_required(VERSION 3.29)
project(pigs)

set(CMAKE_CXX_STANDARD 23)
include(FetchContent)

option(PIGS_OpenGL_Core "Whether Core OpenGL lib is enabled" ON)


###################
# SDL3
###################
find_package(SDL3 REQUIRED)

if(NOT SDL3_FOUND AND NOT EMSCRIPTEN)
    message("Installing and loading SDL3")
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.x
    )
    FetchContent_MakeAvailable(SDL3)
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
find_package(glm REQUIRED)



# ...........................

# ...........................

#########################
# Main compilation
#########################
add_library(pigs
    src/core/Window.cpp
    src/core/Clock.cpp
    src/core/Mouse.cpp
    src/core/callbacks/Callbacks.cpp
    src/core/rendering/textures/TextureRegister.cpp

    src/toolkit/filesystem/filesystem.cpp
    src/toolkit/filesystem/path.cpp
    src/toolkit/OpenGL/Texture.cpp
    src/toolkit/OpenGL/FrameBuffer.cpp
    src/toolkit/types/vec.cpp
    src/toolkit/OpenGL/glShaders.cpp
    src/toolkit/OpenGL/FrameBuffer.cpp
)

target_precompile_headers(pigs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pigs.h
)

target_include_directories(pigs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core
)

target_link_libraries(pigs PUBLIC
    ImGui
    GLAD
    SDL3::SDL3
    stb_image
    glm::glm
)

target_compile_options(pigs PRIVATE
    -Oz
    -fno-rtti
    -flto
)

target_link_options(pigs PRIVATE
    -flto
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
