#include "core/rendering/OpenGL/glShaders.hpp"
// gordie lib
#include <toolkit/filesystem/path.hpp>
#include <toolkit/apidef.h>
#include <core/errors/unwrap.hpp>

// standard lib
#include <vector>
#include <fstream>
#include <sstream>

using namespace pg;

/// Returns false if the shader is not compiled.
static expected<void> verifyProgramIntegrity(GLuint program) {
    // check linked program status
    GLint linked{};
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    // if we're not linked.
    if (!linked) {
        GLint len{};
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len);

        glGetProgramInfoLog(program, len, nullptr, log.data());
        return PG_UErr("verifyProgramIntegrity()", "Shader program linkage failed with error:\n", log.data());
    }
    return{};
}

static expected<void> verifyShaderIntegrity(GLuint shader) {
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

        std::vector<char> log(logLen);
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());

        return PG_UErr("Shader compilation failed with error:\n", log.data());
    }
    return {};
}

static expected<GLuint> compileShader(const char* data, GLenum shaderType, GLsizei length) {
    // then, we set up a shader type
    const GLuint shader = glCreateShader(shaderType);
    // actually specify the source of the shader
    glShaderSource(shader, 1, &data, &length);
    // then we compile the shader.
    glCompileShader(shader);

    PG_ReturnIfErr(verifyShaderIntegrity(shader));
    return shader;
}

static void replaceGLVersionString(std::string& shaderStr) {
    auto index = shaderStr.find("#version");
    auto endIndex = shaderStr.find('\n', index);
    shaderStr.replace(index, endIndex - index + 1, glVersionHeader);
}

expected<GLuint> gl::compileShaderFromPath(GLenum shaderType, const path& pathToShader) {
    if (!std::filesystem::is_regular_file(pathToShader)) {
        return PG_UErr('\"', pathToShader, "\" does not exist or is not a valid file.");
    }
    // load the file
    std::ifstream file(pathToShader);
    if (!file) {
        return PG_UErr("Failed to open shader file \'", pathToShader, "\'");
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
    PG_ReturnExpected(opt);
}

expected<GLuint> gl::makeShaderProgram(
    std::span<const path> vertexShaders,
    std::span<const path> fragmentShaders)
{
    if (vertexShaders.empty()) {
        return PG_UErr("Shader error: must have a minimum of one vertex shader.");
    } if (fragmentShaders.empty()) {
        return PG_UErr("Shader error: must have a minimum of one fragment shader");
    }

    // start creating the program
    auto program = glCreateProgram();

    std::vector<GLuint> shaders;
    shaders.reserve(vertexShaders.size() + fragmentShaders.size());
    for (auto& p: vertexShaders) {
        auto result = compileShaderFromPath(GL_VERTEX_SHADER, p);
        PG_ReturnIfErr(result);
        // if successful attach the shader.
        glAttachShader(program, *result);
        shaders.push_back(*result);
    }
    for (auto& p: fragmentShaders) {
        auto result = compileShaderFromPath(GL_FRAGMENT_SHADER, p);
        PG_ReturnIfErr(result);
        // if successful attach the fragment shader
        glAttachShader(program, *result);
        shaders.push_back(*result);
    }

    // link the program
    glLinkProgram(program);
    // if our program did not compile, return nullopt.
    if (!verifyProgramIntegrity(program)) {
        PG_UErr("Failed to link program ", program, ". Check error log");
    }
    // after linking, we delete all of our shaders.
    for (const auto& sh: shaders)
        glDeleteShader(sh);

    // return the program!
    return program;
}

expected<GLuint> gl::makeShaderProgram(std::initializer_list<path> vertexShaders,
    std::initializer_list<path> fragmentShaders) {
    return makeShaderProgram(std::span(vertexShaders), std::span(fragmentShaders));
}

expected<GLuint> gl::rawMakeShaderProgram(
    std::span<std::pair<char const *, size_t>> vertexShaders,
    std::span<std::pair<char const *, size_t>> fragmentShaders)
{

    if (vertexShaders.empty()) {
        return PG_UErr("Error: Must have a minimum of one vertex shader");
    } if (fragmentShaders.empty()) {
        return PG_UErr("Error: Must have a minimum of one fragment shader");
    }

    // start creating the program
    auto program = glCreateProgram();

    std::vector<GLuint> shaders;
    shaders.reserve(vertexShaders.size() + fragmentShaders.size());
    for (auto& sc: vertexShaders) {
        auto result = compileShader(sc.first, GL_VERTEX_SHADER, static_cast<int>(sc.second));
        PG_ReturnIfErr(result);
        // if successful attach the shader.
        glAttachShader(program, *result);
        shaders.push_back(*result);
    }
    for (auto& sc: fragmentShaders) {
        auto result = compileShader(sc.first, GL_FRAGMENT_SHADER, static_cast<int>(sc.second));
        PG_ReturnIfErr(result);
        // if successful attach the fragment shader
        glAttachShader(program, *result);
        shaders.push_back(*result);
    }

    // link the program
    glLinkProgram(program);
    // if our program did not compile, return nullopt.
    if (!verifyProgramIntegrity(program))
        return PG_UErr("Failed to verify integrity of the shader program");
    // after linking, we delete all of our shaders.
    for (const auto& sh: shaders)
        glDeleteShader(sh);

    // return the program!
    return program;
}


void gl::destroyShader(const GLuint shader) {
    glDeleteShader(shader);
}
