#include <cstring>
#include <algorithm>

#include "core/Engine.hpp"
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <stb_image/stb_image.h>


#include "../../include/core/vitals/EngineStatus.hpp"
#include "toolkit/platform/executable_dir.hpp"

using namespace pg;


Engine& Engine::operator=(Engine&& o) noexcept {
    std::swap(o.window, window);
    keyboard = o.keyboard;
    clock = o.clock;
    mouse = o.mouse;
    textures = std::move(o.textures);
    return *this;
}

void Engine::on_start_loop() {
    glClear(GL_COLOR_BUFFER_BIT); //< Clear the canvas.

    clock.tick(); //< Increment the clock.
    shaders.update(*this);

    // Copy data from SDL into the keyboard struct.
    keyboard.update();
}

void Engine::on_end_loop() {
    mouse.clearMouseWheel(); //< Clear current status of the mouse wheel.

    SDL_GL_SwapWindow(window); //< Swap the window to the user.
}

void Engine::init(int argc, char* argv[]) {
    window = Window::make("Pigs", {300, 300}, WindowHighDPI);
    // Initialize each respective subsystem
    this->textures.init();
    this->shaders.init();
    this->exec.init(argc, argv);
    this->config.init(*this);
    // Set up file & asset management
    std::filesystem::current_path(platform::getExecutableDir());
    stbi_set_flip_vertically_on_load(true);


    // Set the initialization status to be true.
    engine_status.initialized = true;
}
