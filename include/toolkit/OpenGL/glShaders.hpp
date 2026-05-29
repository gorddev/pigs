#pragma once

#include <initializer_list>
#include <optional>
#include <utility>

typedef uint32_t GLuint;
typedef uint32_t GLenum;

namespace pg {

    class path; ///< Forward declaration for the path class.

    namespace gl {
        /// Creates an OpenGL shader program from the provided shaders.
        /// @param vertexShaders The paths to the vertex shaders you want initialized.
        /// @param fragmentShaders The paths to the fragment shaders you want initialized.
        /// @return The ID of the OpenGL shader program.
        std::optional<GLuint> makeShaderProgram(std::initializer_list<path> vertexShaders,
                                                  std::initializer_list<path> fragmentShaders);


        /// Creates an OpenGL shader program from the provided const memory-bound shaders.
        /// @param vertexShaders The raw constant character pointers to the vertex shader strings
        /// @param fragmentShaders The raw constant character pointers to the fragment shader strings.
        /// @return The ID of the OpenGL shader program, if successful.
        std::optional<GLuint> rawMakeShaderProgram(std::initializer_list<std::pair<char const*, size_t>> vertexShaders,
            std::initializer_list<std::pair<char const*, size_t>> fragmentShaders);

        /// Compiles the provided shader with OpenGL
        /// @param shaderType The type of shader you want to compile. Only
        /// fragment and vertex shaders supported.
        /// @param pathToShader the path to the shader
        /// @return A handle to the shader id.
        /// @warning Only fragment and vertex shaders supported.
        std::optional<GLuint> compileShaderFromPath(GLenum shaderType, path pathToShader);

        /// Destroys the given shader from Open GL's context state and VRAM
        /// @param shader The handle of the shader you want to destroy
        void destroyShader(GLuint shader);
    }
}
