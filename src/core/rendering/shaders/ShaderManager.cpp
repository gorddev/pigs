#include <core/rendering/shaders/ShaderManager.hpp>

#include <core/Engine.hpp>

using namespace pg;

void ShaderManager::init() {
    univ_uniforms.init();
}

void ShaderManager::update(const Engine& e) {
    univ_uniforms.updateUniforms(e);
}
