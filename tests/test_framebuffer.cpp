#include "callbacks/AppStatus.hpp"
#include "pigs_init.h"
#include "core/rendering/OpenGL/glShaders.hpp"
#include "core/rendering/OpenGL/VBuffer.hpp"
#include "core/filesystem/path.hpp"

#include <core/rendering/shaders/Shader.hpp>

#include <iostream>
#include "core/rendering/OpenGL/FrameBuffer.hpp"


GLuint shader;
pg::FrameBuffer buffer;
pg::tex_id tex;

pg::Sound soundW;
pg::Sound soundA;
pg::Sound soundS;
pg::Sound soundD;

static constexpr pg::Vertex vertices[] = {
    {0, 0, 0, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 1, 0, 1, 1},
    {0, 0, 0, 0, 0},
    {0, 1, 0, 0, 1},
    {1, 1, 0, 1, 1},
};

pg::VBuffer<> vb;


pg::AppStatus loop(pg::Engine& e){

    buffer.bind();


    GL_CHECK();
    glUseProgram(shader);
    GL_CHECK();
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, pg::UniversalUniforms::globalUbo);
    vb.glBind()
      .glDraw()
      .glUnbind();

    glBindVertexArray(0);
    GL_CHECK();

    static glm::vec2 pos{};
    glm::vec2 vel{
        (e.keyboard.isHeld('D') - e.keyboard.isHeld('A')) * 0.2f,
        (e.keyboard.isHeld('W') - e.keyboard.isHeld('S')) * 0.2f
    };

    if (e.keyboard.isTapped('W')) {
        e.audio.start_sound(soundW);
    } if (e.keyboard.isTapped('A')) {
        e.audio.start_sound(soundA);
    } if (e.keyboard.isTapped('D')) {
        e.audio.start_sound(soundD);
    } if (e.keyboard.isTapped('S')) {
        e.audio.start_sound(soundS);
    } if (e.keyboard.isTapped(' ')) {
        e.config.vsync = !e.config.vsync;
    }

    if (e.keyboard.isTapped(pg::KEY_UP)) {
        e.config.audio.music_volume += 0.1;
    } if (e.keyboard.isTapped(pg::KEY_DOWN)) {
        e.config.audio.music_volume -= 0.1;
    }

    // 2. Safely normalize and step using a branchless length squared check
    float lenSq = glm::dot(vel, vel);
    if (lenSq > 0.0001f) {
        pos += (vel * glm::inversesqrt(lenSq)) * 0.2f * e.clock.dtf;
    }


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    buffer.blit({(int)pos.x, (int)pos.y, 300, 300});

    return pg::APP_CONTINUE;
}

pg::AppStatus init(pg::Engine& e) {
  //First set up some defaults for the engine.
  e.config.assets_folder = "../../tests/assets";
  e.config.vsync = false;
  // Make sure we don't get a warning about no event/crash functino
  e.config.warn.init = {
  	.no_event_func = false,
   	.no_quit_func  = false,
   	.no_crash_func = false
  };

  // Set some properties of our window.
  e.window.setResizable(true)
    .setFloatOnTop(true)
    .setName("my game")
    .setDimensions({800, 800})
    .setPosition({0, 0});

  vb = pg::VBuffer<>::make(vertices, 6);

  auto texExp = e.textures.create2D("img.png", pg::PG_PIXEL);
  pg::tex_id tex;
  if (texExp) tex = *texExp;
  else {
    std::cerr << texExp.error().what() << std::endl;
    std::cerr << texExp.error().display_str() << std::endl;
    return pg::APP_QUIT;
  }

  tex = PG_Unwrap(texExp);
  buffer = PG_Unwrap(pg::FrameBuffer::make(100, 100, pg::PG_PIXEL));

  e.setLoop(loop);
  e.onCrash([](pg::Engine&){
    std::puts("We've got a crashing callback active~");
  });

  auto s1 = pg::path("std.vert");
  auto s2 = pg::path("std.frag");
  auto shad = pg::Shader({{s1}}, {{s2}});
  shader = PG_Unwrap(pg::gl::makeShaderProgram({{s1}}, {{s2}}));

  e.textures.glBind(tex);

  soundA = e.audio.init_sfx<pg::Music>("expA.wav");
  soundS = e.audio.init_sfx<pg::Music>("expS.wav");
  soundD = e.audio.init_sfx<pg::Music>("expD.wav");
  soundW = e.audio.init_sfx<pg::Music>("expW.wav");

  [[maybe_unused]] auto soundTest = e.audio.init_sfx<pg::Music>("std.vert");

  return pg::AppStatus::APP_CONTINUE;
}
