#pragma once

#include <string>

namespace pg::err {

	struct GLShaderLinkErr {
		std::string gl_log;
		std::string str() const {
			return std::string("OpenGL Failed to link program with error:\n") + gl_log;
		}
	};

	struct GLShaderCompileErr {
		std::string gl_log;
		std::string str() const {
			return std::string("OpenGL Shader compilation failed with error:\n") + gl_log;
		}
	};

	struct GLNoShaderProvided {
		enum ShaderType {
			VERTEX_SHADER,
			FRAGMENT_SHADER
		} shader_type;
		std::string str() const {
			std::string shader_type_str;
			switch(this->shader_type) {
			case VERTEX_SHADER:
				shader_type_str = "vertex shader";
			break;
			case FRAGMENT_SHADER:
				shader_type_str = "fragment shader";
			}
			return std::string("No ") + shader_type_str + "s provided to shader program creation function.";
		}
	};

}
