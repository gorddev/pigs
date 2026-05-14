/* Created by Gordie Novak on 3/22/26.
 * Purpose: 
 *
//x_n+1 = sin(ay_n) + c cos(a x_n);
//y_{n+1} = sin(b x_n) + d cos (b y_n); */

#include "GL_Init.h"
#include "filesystem/filesystem.hpp"
#include "types/dim2.hpp"
#include "window/Window.hpp"


constexpr uint32_t num_vertices = 1000000;

int main() {

    const float* vertex_data = new float[num_vertices*3];

    gan::Window window = gan::Window::makeGL("My window", {400, 300}, gan::WindowResizable | gan::WindowFloatOnTop);
    gan::files::set_assets_folder("assets_old");

    auto program = create_GL_program("assets_old/popcorn.vert", "assets_old/popcorn.frag");

    auto vao = create_GL_vertex_array(vertex_data, num_vertices);

    SDL_Event e;

    bool running = true;
    while (running) {

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_EVENT_QUIT:
                running = false;
            default:
                break;
            }
        }



        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, num_vertices);

    }



    delete[] vertex_data;


}
