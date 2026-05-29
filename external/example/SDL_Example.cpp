#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

#include "../SDL_API.h"
#include "GL_Init.h"
#include "external/imgui/imgui.h"
#include "external/imgui/backends/imgui_impl_opengl3.h"
#include "external/imgui/backends/imgui_impl_sdl3.h"

GLuint vao;
GLuint program;
gan::Window winGL = gan::Window::makeGL("My GL Context", {340, 300}, gan::WindowFloatOnTop | gan::WindowResizable);
gan::Clock timer = gan::Clock::create();

GLint uResolution, uMousePos, uIterations, uScale, uStartpos;

ImGuiContext* imgui;

SDL_AppResult SDL_AppInit(void** userdata, int argc, char* argv[]) {
    int x, y;

    gan::files::set_assets_folder("assets");

    winGL.setPosition({560, 300});
    const float vertices[] = {
        -1.f,-1.f,0.f,
        -1.f, 1.f,0.f,
         1.f, 1.f,0.f,
        -1.f, -1.f, 0.f,
        1.f, -1.f, 0.f,
        1.f, 1.f, 0.0,
    };

    vao = create_GL_vertex_array(vertices, 6);
    program = create_GL_program(gan::files::assets().std()/"julia.vert", gan::files::assets().std()/"julia.frag");

    uResolution = glGetUniformLocation(program, "uResolution");
    uMousePos = glGetUniformLocation(program, "uMousepos");
    uIterations = glGetUniformLocation(program, "uIterations");
    uScale = glGetUniformLocation(program, "scale");
    uStartpos = glGetUniformLocation(program, "uStartpos");

    imgui = ImGui::CreateContext();
    ImGui_ImplSDL3_InitForOpenGL(winGL, winGL.getGLContext());
    ImGui_ImplOpenGL3_Init("#version 410 core");



    return SDL_APP_CONTINUE;
}



SDL_AppResult SDL_AppIterate(void* userdata) {

    ImGui::SetCurrentContext(imgui);
    ImGui_ImplSDL3_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    static uint32_t tick = SDL_GetTicks();
    const uint32_t new_tick = SDL_GetTicks();
    const uint32_t dt = new_tick - tick;
    tick = new_tick;


    glClear(GL_COLOR_BUFFER_BIT);

    static int w, h;
    static bool _ = SDL_GetWindowSizeInPixels(winGL, &w ,&h);
    int width, height;
    SDL_GetWindowSizeInPixels(winGL, &width, &height);

    // resolution uniform
    glViewport(0, 0, width, height);
    glUniform2f(uResolution, float(width), float(height));

    // mouse pos uniform
    float x, y;
    SDL_GetMouseState(&x, &y);
    //winGL.normalizeMousePosToWindow(x, y);

    static float tx = x, ty = y;
    tx += (x-tx)*0.08*dt;
    ty += (y-ty)*0.08;
    glUniform2f(uMousePos, x, y);

    // iteration uniform
    static int iteration_uni = 30;
    static float scale = 3.0;
    static float spos[2] = {0.0, 0.0};
    ImGui::Begin("ittt");
    ImGui::SliderInt("iteration", &iteration_uni, 2, 2000);
    ImGui::SliderFloat("scale", &scale, 0.00013, 5);
    ImGui::SliderFloat2("sx, sy", spos, -1200, 1200);
    ImGui::Text("Mouse adj pos: %f, %f", tx,ty);
    ImGui::Text("Mouse norm pos: %f, %f", x, y);
    ImGui::Text("My window size: %i, %i", winGL.getWidth(), winGL.getHeight());
    ImGui::End();
    glUniform1i(uIterations, iteration_uni);
    glUniform1f(uScale, scale);
    glUniform2f(uStartpos, -spos[0], spos[1]);


    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    SDL_GL_SwapWindow(winGL);

    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void* userdata, SDL_Event* event) {
    ImGui_ImplSDL3_ProcessEvent(event);
    switch (event->type) {
    case SDL_EVENT_QUIT:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_WINDOW_RESIZED:
        winGL.on_SDLWindowEvent(*event);
    default:
        break;
    }
    return SDL_APP_CONTINUE;


}

void SDL_AppQuit(void* userdata, SDL_AppResult result) {}


