#include <glm/ext/matrix_clip_space.hpp>
#include <rendering/shaders/universal/UniversalUniforms.hpp>
#include <toolkit/apidef.h>

#include "Engine.hpp"

using namespace pg;

GLuint UniversalUniforms::globalUbo = 0;

void UniversalUniforms::init() {
    glGenBuffers(1, &globalUbo);

    if (globalUbo==0) {
        PG_Panic("GlobalUBO not set correctly");
    }
    glBindBuffer(GL_UNIFORM_BUFFER, globalUbo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(*this), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalUbo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GL_CHECK();
}

void UniversalUniforms::updateUniforms(const Engine& e) {
    screenProjection = glm::ortho(0.0f, e.window.getWidth(), 0.0f, e.window.getHeight(), -1.0f, 1.0f);
    resolution = std::bit_cast<glm::vec2>(e.window.getDimensions());
    frame = e.clock.frame;
    time = e.clock.ftime;

    GL_CHECK();
    glBindBuffer(GL_UNIFORM_BUFFER, globalUbo);
    GL_CHECK();
    glBufferData(GL_UNIFORM_BUFFER, sizeof(*this), nullptr, GL_DYNAMIC_DRAW);
    GL_CHECK();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(*this), this);
    GL_CHECK();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    GL_CHECK();

}

