#pragma once
#include "universal/UniversalUniforms.hpp"

/* Created by Gordie Novak on 8/21/26.
 * Purpose: 
 */

namespace pg {

    class Engine;

    class ShaderManager {
    public:

        friend class Engine;

        void init();
        void update(const Engine& e);

        UniversalUniforms univ_uniforms;

    };

}