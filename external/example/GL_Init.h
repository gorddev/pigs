#pragma once

#include "../core/apidef.h"
#include <filesystem>
#include <iostream>

GLuint create_GL_vertex_array(const float vertices[], size_t num_vertices);
GLuint create_GL_program(const std::__fs::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader);

inline GLuint create_GL_vertex_array(const float vertices[], size_t num_vertices) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float)*num_vertices*3,
        vertices,
    GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    return vao;
}


#include <sstream>
#include <fstream>
inline GLuint create_GL_program(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader) {

    GLuint program = glCreateProgram();
    glUseProgram(program);

    if (!std::filesystem::exists(vertexShader)) {
        std::cerr << "Err: Vertex shader not found" << std::endl;
        throw std::runtime_error("Vertex shader not found");
    } if (!std::filesystem::exists(fragmentShader)) {
        std::cerr << "Err: Fragment shader not found" << std::endl;
        throw std::runtime_error("Fragment shader not found");
    }

    auto unsafe_compile_shader = [](const std::filesystem::path& path, uint64_t compile_type) {
        std::ifstream shader_data(path);

        std::stringstream ss;
        ss << shader_data.rdbuf();

        std::string s = ss.str();
        const char* sctr = s.c_str();
        GLint len = s.length();
        GLuint shader = glCreateShader(compile_type);
        glShaderSource(shader, 1, &sctr, &len);
        glCompileShader(shader);

        GLint result;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
        if (result <= 0) {
            GLint log_length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
            char buff[log_length];
            glGetShaderInfoLog(shader, log_length, nullptr, buff);
            std::string log = "Shader compilation error: ";
            log += buff;
            printf("log len: %i", log_length);
            throw std::runtime_error(log);
        }

        ss.clear();
        s.clear();
        shader_data.close();

        return shader;
    };


    auto v_shader = unsafe_compile_shader(vertexShader, GL_VERTEX_SHADER);
    glAttachShader(program, v_shader);
    auto f_shader = unsafe_compile_shader(fragmentShader, GL_FRAGMENT_SHADER);
    glAttachShader(program, f_shader);

    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked <= 0) {
        GLint log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        char buff[log_length];
        glGetProgramInfoLog(program, log_length, nullptr, buff);
        std::string log = "Shader compilation error: ";
        log += buff;
        throw std::runtime_error(log);
    }

    glDeleteShader(v_shader);
    glDeleteShader(f_shader);

    return program;
}
