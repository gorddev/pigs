#pragma once
#include <glm/mat4x4.hpp>
#include <meta>
#include <toolkit/intdef.h>
#include <string>
#include <string_view>
#include <array>
#include <algorithm>

/* Created by Gordie Novak on 8/20/26.
 * Purpose:
 * Contains data to universalize all uniforms in accordance with universal_uniforms.glsl*/

namespace pg {

    class Engine;

    constexpr char universal_block_header[] =
        "pg_SceneData";

    struct UniversalUniforms {
        glm::mat4 screenProjection;
        glm::vec2 resolution;
        u32       frame;
        float     time;

    public:
        static GLuint globalUbo;
        friend class ShaderManager;
        void init();
        void updateUniforms(const Engine& e);
    };

    #include "GenerateUniversalGLSL.inl"

    namespace internal_constants {
        static constexpr auto universal_uniforms_arr =
            generateGLSLBlock();
    }
    constexpr const char* universal_uniforms =
        internal_constants::universal_uniforms_arr.data();




}
