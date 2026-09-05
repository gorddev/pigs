#include "core/rendering/OpenGL/glShaders.hpp"
// gordie lib
#include <core/filesystem/path.hpp>
#include <toolkit/apidef.h>
#include <core/errors/unwrap.hpp>

// standard lib
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include "errors/Error.hpp"
#include "errors/error-structs/OpenGL/GLShaderErrors.hpp"
#include "errors/error-structs/files/FileNotExistsError.hpp"
#include "errors/error-structs/files/FileNotOpened.hpp"
#include "rendering/shaders/universal/UniversalUniforms.hpp"
#include "toolkit/serialization/concepts/is_specialization_of.hpp"

using namespace pg;

static expected<
	GLuint,
	err::GLShaderCompileErr>
compileShader(std::string_view data, GLenum shaderType) {
  // then, we set up a shader type
  const GLuint shader = glCreateShader(shaderType);
  // actually specify the source of the shader
  const char* data_ptr = data.data();
  const GLsizei data_len = data.length();
  glShaderSource(shader, 1, &data_ptr, &data_len);
  // then we compile the shader.
  glCompileShader(shader);
  // Then we check whether the shader compilation was successful or not.
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
      GLint logLen = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
      std::string log;
      log.resize(logLen);
      glGetShaderInfoLog(shader, logLen, nullptr, log.data());
      return PG_UErrNew(err::GLShaderCompileErr, .gl_log = std::move(log));
  }
  // Otherwise return the real shader.
  return shader;
}

static inline void replaceGLVersionString(std::string& shaderStr) {
    auto index = shaderStr.find("#version");
    auto endIndex = shaderStr.find('\n', index);
    shaderStr = std::string(glVersionHeader) + universal_uniforms + shaderStr.substr(endIndex);
}

static expected<
	GLuint,
	err::FileNotExists,
	err::FileNotOpened,
	err::GLShaderCompileErr,
	err::GLShaderLinkErr>
compileShaderFromPath(GLenum shaderType, const path& pathToShader) {
    if (!std::filesystem::is_regular_file(pathToShader))
    	return PG_UErrNew(err::FileNotExists, .file_name = pathToShader.c_str());
    // load the file
    std::ifstream file(pathToShader);
    if (!file)
    	return PG_UErrNew(err::FileNotOpened, .file_path = pathToShader.c_str());
    std::stringstream ss;
    // load the entire file into the string stream.
    ss << file.rdbuf();
    file.close();

    std::string file_str = ss.str();
    replaceGLVersionString(file_str);

    auto opt = compileShader(file_str, shaderType);
    PG_ReturnIfUErr(opt);
    return *opt;
}

expected<
	GLuint,
	err::FileNotExists,
	err::FileNotOpened,
	err::GLNoShaderProvided,
	err::GLShaderCompileErr,
	err::GLShaderLinkErr>
gl::makeShaderProgram(
    std::span<const path> vertexShaders,
    std::span<const path> fragmentShaders)
{
  if (vertexShaders.empty()) {
    return PG_UErrNew(err::GLNoShaderProvided, .shader_type = err::GLNoShaderProvided::VERTEX_SHADER);
  } if (fragmentShaders.empty()) {
    return PG_UErrNew(err::GLNoShaderProvided, .shader_type = err::GLNoShaderProvided::FRAGMENT_SHADER);
  }

  // start creating the program
  auto program = glCreateProgram();

  std::vector<GLuint> shaders;
  shaders.reserve(vertexShaders.size() + fragmentShaders.size());
  auto delete_shaders = [&shaders](){
   	for (auto& s : shaders) {
    		glDeleteShader(s);
   	}
  };

  for (auto& p: vertexShaders) {
      auto result = compileShaderFromPath(GL_VERTEX_SHADER, p);
      // If unsuccessful return the error
      if (!result) {
       	delete_shaders();
       	PG_ReturnUErrUnsafe(result);
      }
      // if successful attach the shader.
      glAttachShader(program, *result);
      shaders.push_back(*result);
  }
  for (auto& p: fragmentShaders) {
      auto result = compileShaderFromPath(GL_FRAGMENT_SHADER, p);
      if (!result) {
       	delete_shaders();
       	PG_ReturnUErrUnsafe(result);
      }
      // if successful attach the fragment shader
      glAttachShader(program, *result);
      shaders.push_back(*result);
  }

  // link the program
  glLinkProgram(program);
  // check linked program status
  GLint linked{};
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  // if we're not linked.
  if (!linked) {
      GLint len{};
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
      std::string log;
      log.resize(len);

      glGetProgramInfoLog(program, len, nullptr, log.data());
      delete_shaders();
      return PG_UErrNew(err::GLShaderLinkErr, .gl_log = std::move(log));
  }
  // after linking, we delete all of our shaders.
  delete_shaders();

  // Then we bind our universal uniforms to slot 0
  GLuint blockIndex = glGetUniformBlockIndex(program, universal_block_header);
  if (blockIndex != GL_INVALID_INDEX)
      glUniformBlockBinding(program, blockIndex, 0);

  // return the program!
  return program;
}

expected<
	GLuint,
	err::FileNotExists,
	err::FileNotOpened,
	err::GLNoShaderProvided,
	err::GLShaderCompileErr,
	err::GLShaderLinkErr>
gl::makeShaderProgram(
	std::initializer_list<path> vertexShaders,
  std::initializer_list<path> fragmentShaders)
{
    return makeShaderProgram(std::span(vertexShaders), std::span(fragmentShaders));
}

expected<
	GLuint,
	err::FileNotExists,
	err::FileNotOpened,
	err::GLNoShaderProvided,
	err::GLShaderCompileErr,
	err::GLShaderLinkErr>
gl::rawMakeShaderProgram(
    std::span<std::string_view> vertexShaders,
    std::span<std::string_view> fragmentShaders)
{

	if (vertexShaders.empty()) {
      return PG_UErrNew(err::GLNoShaderProvided, .shader_type = err::GLNoShaderProvided::VERTEX_SHADER);
    } if (fragmentShaders.empty()) {
      return PG_UErrNew(err::GLNoShaderProvided, .shader_type = err::GLNoShaderProvided::FRAGMENT_SHADER);
    }

    // start creating the program
    auto program = glCreateProgram();

    std::vector<GLuint> shaders;
    shaders.reserve(vertexShaders.size() + fragmentShaders.size());
    auto delete_shaders = [&shaders](){
    	for (auto& s : shaders) {
     		glDeleteShader(s);
     	}
    };

    for (auto& p: vertexShaders) {
        auto result = compileShader(p, GL_VERTEX_SHADER);
        // If unsuccessful return the error
        if (!result) {
        	delete_shaders();
         	PG_ReturnUErrUnsafe(result);
        }
        // if successful attach the shader.
        glAttachShader(program, *result);
        shaders.push_back(*result);
    }
    for (auto& p: fragmentShaders) {
        auto result = compileShader(p, GL_FRAGMENT_SHADER);
        if (!result) {
        	delete_shaders();
         	PG_ReturnUErrUnsafe(result);
        }
        // if successful attach the fragment shader
        glAttachShader(program, *result);
        shaders.push_back(*result);
    }

    // link the program
    glLinkProgram(program);
    // check linked program status
    GLint linked{};
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    // if we're not linked.
    if (!linked) {
        GLint len{};
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        std::string log;
        log.resize(len);

        glGetProgramInfoLog(program, len, nullptr, log.data());
        delete_shaders();
        return PG_UErrNew(err::GLShaderLinkErr, .gl_log = std::move(log));
    }
    // after linking, we delete all of our shaders.
    delete_shaders();

    // Then we bind our universal uniforms to slot 0
    GLuint blockIndex = glGetUniformBlockIndex(program, universal_block_header);
    if (blockIndex != GL_INVALID_INDEX)
        glUniformBlockBinding(program, blockIndex, 0);

    // return the program!
    return program;
}


void gl::destroyShader(const GLuint shader) {
    glDeleteShader(shader);
}
