#include <core/rendering/shaders/universal/UniversalUniforms.hpp>

#include <glm/ext/matrix_clip_space.hpp>
#include <toolkit/apidef.h>

#include <core/Engine.hpp>

#include "GenerateUniversalGLSL.inl"

using namespace pg;

GLuint UniversalUniforms::globalUbo = 0;
constexpr auto char_data = generateGLSLBlock();

#ifdef __EMSCRIPTEN__
std::string_view pg::universal_uniforms = {char_data, sizeof(char_data)-1};
#else
std::string_view pg::universal_uniforms = {std::data(char_data), sizeof(char_data)-1};
#endif


void UniversalUniforms::init() {
    glGenBuffers(1, &globalUbo);

    if (globalUbo==0) {
        PG_Panic("GlobalUBO not set correctly");
    }
    glBindBuffer(GL_UNIFORM_BUFFER, globalUbo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(*this), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalUbo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniversalUniforms::updateUniforms(const Engine& e) {
    screenProjection = glm::ortho(0.0f, e.window.getWidth(), 0.0f, e.window.getHeight(), -1.0f, 1.0f);
    resolution = std::bit_cast<glm::vec2>(e.window.getDimensions());
    frame = e.clock.frame;
    time = e.clock.ftime;

    glBindBuffer(GL_UNIFORM_BUFFER, globalUbo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(*this), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(*this), this);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

}
