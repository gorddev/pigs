#include "toolkit/OpenGL/glShaders.hpp"
// gordie lib
#include <toolkit/filesystem/path.hpp>
#include <toolkit/apidef.h>
#include <core/errors/pig_log.hpp>

// standard lib
#include <vector>
#include <fstream>
#include <sstream>

using namespace pg;

/// Returns false if the shader is not compiled.
static bool verifyProgramIntegrity(GLuint program) {
    // check linked program status
    GLint linked{};
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    // if we're not linked.
    if (!linked) {
        GLint len{};
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len);

        glGetProgramInfoLog(program, len, nullptr, log.data());
        PIG_WriteLog("verifyProgramIntegrity()", "Shader program linkage failed with error:\n", log.data());
        return false;
    }
    return true;
}

static bool verifyShaderIntegrity(GLuint shader) {
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

        ::std::vector<char> log(logLen);
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());

        PIG_WriteLog("verifyShaderIntegrity()", "Shader compilation failed with error:\n", log.data());
        return false;
    }
    return true;
}

static std::optional<GLuint> compileShader(const char* data, GLenum shaderType, GLsizei length) {
    // then, we set up a shader type
    const GLuint shader = glCreateShader(shaderType);
    // actually specify the source of the shader
    glShaderSource(shader, 1, &data, &length);
    // then we compile the shader.
    glCompileShader(shader);
    if (!verifyShaderIntegrity(shader)) {
        return std::nullopt;
    }
    return shader;
}

static void replaceGLVersionString(std::string& shaderStr) {
    auto index = shaderStr.find("#version");
    auto endIndex = shaderStr.find('\n', index);
    shaderStr.replace(index, endIndex - index + 1, glVersionHeader);
}

std::optional<GLuint> gl::compileShaderFromPath(GLenum shaderType, path pathToShader) {

    if (!pathToShader.is_regular_file()) {
        PIG_AppendLog("gl::compileShaderFromPath()", "Path provided does not exist.");
        return std::nullopt;
    }
    // load the file
    std::ifstream file(pathToShader.expand().c_str());
    if (!file) {
        PIG_WriteLog("gl::compileShaderFromPath()","Failed to open shader file \'", pathToShader.expand().c_str(), "\'");
        return std::nullopt;
    }
    std::stringstream ss;
    // load the entire file into the string stream.
    ss << file.rdbuf();
    file.close();

    std::string file_str = ss.str();
    replaceGLVersionString(file_str);

    const char* data = file_str.c_str(); //< store the data in a const* for func
    auto length = static_cast<GLsizei>(file_str.length());

    auto opt = compileShader(data, shaderType, length);
    if (!opt)
        PIG_AppendLog("gl::compileShaderFromPath()", "Shader verification failed.");
    return opt;
}

std::optional<GLuint> gl::makeShaderProgram(
    std::initializer_list<path> vertexShaders,
    std::initializer_list<path> fragmentShaders)
{
    if (vertexShaders.size() <= 0) {
        PIG_WriteLog("gl::makeShaderProgram()", "Shader error: must have a minimum of one vertex shader.");
        return std::nullopt;
    } else if (fragmentShaders.size() <= 0) {
        PIG_WriteLog("gl::makeShaderProgram()", "Shader error: must have a minimum of one fragment shader.");
        return std::nullopt;
    }

    // start creating the program
    auto program = glCreateProgram();

    std::vector<GLuint> shaders;
    shaders.reserve(vertexShaders.size() + fragmentShaders.size());
    for (auto& p: vertexShaders) {
        auto result = compileShaderFromPath(GL_VERTEX_SHADER, p);
        if (!result) {
            PIG_AppendLog("gl::makeShaderProgram()", "Bad result from gl::compileShaderFromPath() for shader ", "\'", p.c_str(), "\'");
            return std::nullopt;
        }
        // if successful attach the shader.
        glAttachShader(program, result.value());
        shaders.push_back(result.value());
    }
    for (auto& p: fragmentShaders) {
        auto result = compileShaderFromPath(GL_FRAGMENT_SHADER, p);
        if (!result) {
            PIG_RawAppendLog() << "Failed to compile fragment shader: " << p << ".";
            return std::nullopt;
        }
        // if successful attach the fragment shader
        glAttachShader(program, result.value());
        shaders.push_back(result.value());
    }

    // link the program
    glLinkProgram(program);
    // if our program did not compile, return nullopt.
    if (!verifyProgramIntegrity(program)) {
        PIG_AppendLog("gl::makeShaderProgram()", "Failed to link program ", (int) program, ". Check error log.");
        return std::nullopt;
    }
    // after linking, we delete all of our shaders.
    for (const auto& sh: shaders)
        glDeleteShader(sh);

    // return the program!
    return program;
}

std::optional<GLuint> gl::rawMakeShaderProgram(
    std::initializer_list<std::pair<char const *, size_t>> vertexShaders,
    std::initializer_list<std::pair<char const *, size_t>> fragmentShaders)
{

    if (vertexShaders.size() <= 0) {
        PIG_WriteLog("gl::CreateShaderProgram: Must have a minimum of one vertex shader.");
        return std::nullopt;
    } else if (fragmentShaders.size() <= 0) {
        PIG_WriteLog("gl::CreateShaderProgram: Must have a minimum of one fragment shader.");
        return std::nullopt;
    }

    // start creating the program
    auto program = glCreateProgram();

    std::vector<GLuint> shaders;
    shaders.reserve(vertexShaders.size() + fragmentShaders.size());
    for (auto& sc: vertexShaders) {
        auto result = compileShader(sc.first, GL_VERTEX_SHADER, sc.second);
        if (!result) {
            PIG_RawAppendLog() << "Failed to compile static vertex shader: " << sc.first << ".";
            return std::nullopt;
        }
        // if successful attach the shader.
        glAttachShader(program, result.value());
        shaders.push_back(result.value());
    }
    for (auto& sc: fragmentShaders) {
        auto result = compileShader(sc.first, GL_FRAGMENT_SHADER, sc.second);
        if (!result) {
            PIG_RawAppendLog() << "Failed to compile static fragment shader: " << sc.first << ".";
            return std::nullopt;
        }
        // if successful attach the fragment shader
        glAttachShader(program, result.value());
        shaders.push_back(result.value());
    }

    // link the program
    glLinkProgram(program);
    // if our program did not compile, return nullopt.
    if (!verifyProgramIntegrity(program))
        return std::nullopt;
    // after linking, we delete all of our shaders.
    for (const auto& sh: shaders)
        glDeleteShader(sh);

    // return the program!
    return program;
}


void gl::destroyShader(GLuint shader) {
    glDeleteShader(shader);
}
