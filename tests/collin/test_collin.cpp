#include "core/rendering/OpenGL/vertices/TextureVertex.hpp"
#include "core/rendering/OpenGL/VBuffer.hpp"
#include "toolkit/apidef.h"
#include <pigs.h>
#include <pigs_init.h>

#include <iostream>

#include "Collin.hpp"

static constexpr pg::Vertex vbuffer_vertices[] = {
    {0, 0, 0, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 1, 0, 1, 1},
    {0, 0, 0, 0, 0},
    {0, 1, 0, 0, 1},
    {1, 1, 0, 1, 1},
};

pg::VBuffer<> vb;
pg::tex_id mtex;
GLuint test_shader;



#include <core/rendering/OpenGL/glShaders.hpp>
pg::VBuffer<pg::TVertex> screen_span;
GLuint program;

pg::AppStatus init(pg::Engine& e) {
	e.config.warn.init = {
  	.no_event_func = false,
   	.no_quit_func = false,
   	.no_crash_func = false
  };
	e.config.vsync = true;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	e.config.assets_folder = "../../tests/assets/collin";

	e.window
		.setResizable(true)
		.setFloatOnTop(true);

	// first creat our vector of vertices
	screen_span = screen_span.make(pg::TVertex::full_quad({-1.f,-1.f}, {1.f,1.f}).data(), 6);

	vb = pg::VBuffer<>::make(vbuffer_vertices, 6);

	mtex = PG_Unwrap(e.textures.create2D("../img.png", pg::PG_PIXEL));

	// then initialize our buffer
	//collin_green.init();

	// create our shader
	program = PG_Unwrap(pg::gl::makeShaderProgram(
		{pg::path("c1.vert")},
		{pg::path("c1.frag")})
	);
	GL_CHECK();


	//collin_green.void_texture();
	static Collin green(400, 400, 5);
	green.init();

	e.setLoop([&green](pg::Engine& e) mutable {

		if (e.clock.frame % 16 == 0) {
			green.tick({1, 0, 0, 1, 0, 0, 1, 1});
		}

		if (e.clock.frame % 500 == 0) {
			std::cerr << "Dt = " << e.clock.dt << std::endl;
			std::cerr << "fps = " << 1000000000.f/ e.clock.dt << std::endl;
		}

		glUseProgram(program);
		green.glBindTex();
		screen_span
			.glBind()
			.glDraw()
			.glUnbind();

		static bool color = false;

		if (e.clock.frame % 1 == 0) {
			color = !color;
			glClearColor(
	    color ? 1.0f : 0.0f,
	    color ? 1.0f : 0.0f,
	    color ? 1.0f : 0.0f,
	    1.0f
			);
		}

		glClear(GL_COLOR_BUFFER_BIT);


		return pg::APP_CONTINUE;
	});

	return pg::APP_CONTINUE;
}
