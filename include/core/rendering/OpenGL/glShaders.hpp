#pragma once

#include <span>
#include <optional>
#include <utility>

#include <core/errors/unwrap.hpp>
#include <core/errors/err-types/OpenGL/GLShaderErrors.hpp>
#include <core/errors/err-types/files/FileErrors.hpp>



typedef uint32_t GLuint;
typedef uint32_t GLenum;

namespace pg {

    class path; ///< Forward declaration for the path class.

    namespace gl {
        /// Creates an OpenGL shader program from the provided shaders.
        /// @param vertexShaders The paths to the vertex shaders you want initialized.
        /// @param fragmentShaders The paths to the fragment shaders you want initialized.
        /// @return The ID of the OpenGL shader program.
        expected<
					GLuint,
					err::FileNotExists,
					err::FileNotOpened,
					err::GLNoShaderProvided,
					err::GLShaderCompileErr,
					err::GLShaderLinkErr>
				makeShaderProgram(
            std::span<const path> vertexShaders,
            std::span<const path> fragmentShaders
        );

        /// Creates an OpenGL shader program from the provided shaders.
        /// @param vertexShaders The paths to the vertex shaders you want initialized.
        /// @param fragmentShaders The paths to the fragment shaders you want initialized.
        /// @return The ID of the OpenGL shader program.
        expected<
					GLuint,
					err::FileNotExists,
					err::FileNotOpened,
					err::GLNoShaderProvided,
					err::GLShaderCompileErr,
					err::GLShaderLinkErr>
				makeShaderProgram(
            std::initializer_list<path> vertexShaders,
            std::initializer_list<path> fragmentShaders
        );

        /// Creates an OpenGL shader program from the provided const memory-bound shaders.
        /// @param vertexShaders The raw constant character pointers to the vertex shader strings
        /// @param fragmentShaders The raw constant character pointers to the fragment shader strings.
        /// @return The ID of the OpenGL shader program, if successful.
        expected<
					GLuint,
					err::FileNotExists,
					err::FileNotOpened,
					err::GLNoShaderProvided,
					err::GLShaderCompileErr,
					err::GLShaderLinkErr>
				rawMakeShaderProgram(
            std::span<std::string_view> vertexShaders,
            std::span<std::string_view> fragmentShaders
        );

        /// Destroys the given shader from Open GL's context state and VRAM
        /// @param shader The handle of the shader you want to destroy
        void destroyShader(GLuint shader);
    }
}
