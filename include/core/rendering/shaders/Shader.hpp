#pragma once

/* Created by Gordie Novak on 7/31/26.
 * Purpose: 
 * Defines the usage for a shader and its respective uniforms */
#include <toolkit/apidef.h>
#include <string>

#include <span>

#include <core/filesystem/dumpFile.hpp>

#include "core/rendering/OpenGL/glShaders.hpp"
#include <print>
#include <ranges>

#include "toolkit/containers/hash_map.hpp"

namespace pg {

    struct Shader {
        /// Maps each uniform to the uniform location
        string_map<GLint> uniforms;
        /// The actual open-gl program tied to this shader.
        GLint program;


        Shader(std::span<const path> vertexShaders, std::span<const path> fragmentShaders) {
            init(vertexShaders, fragmentShaders);
        }

    private:
        void init(std::span<const path> vertexShaders, std::span<const path> fragmentShaders) {
            auto is_id_char = [](char c) {
                return std::isalnum(c) || c == '_';
            };

            auto parse_uniforms = [&is_id_char](const std::filesystem::path& path, string_map<GLint>& uniforms) {
                auto size = std::filesystem::file_size(path);
                std::string content(size, '\0');
                std::ifstream file(path, std::ios::binary);
                file.read(content.data(), size);

                std::string_view src(content);
                size_t i = 0;

                while (i < src.size()) {
                    if (src[i] == '/' && i + 1 < src.size() && src[i + 1] == '*') {
                        i += 2;
                        while (i + 1 < src.size() && !(src[i] == '*' && src[i + 1] == '/')) { i++; }
                        i += 2; continue;
                    }
                    if (src[i] == '/' && i + 1 < src.size() && src[i + 1] == '/') {
                        while (i < src.size() && src[i] != '\n') { i++; }
                        continue;
                    }

                    if (src.substr(i, 7) == "uniform" && (i + 7 == src.size() || !is_id_char(src[i + 7]))) {
                        i += 7;

                        while (i < src.size() && std::isspace(src[i])) { i++; }
                        while (i < src.size() && is_id_char(src[i])) { i++; }
                        while (i < src.size() && std::isspace(src[i])) { i++; }

                        size_t name_start = i;
                        while (i < src.size() && is_id_char(src[i])) { i++; }

                        if (name_start != i) {
                            std::string_view uniform_name = src.substr(name_start, i - name_start);

                            uniforms.emplace(uniform_name, -1);
                        }

                    }
                    i++;
                }
            };

            for (auto& path: vertexShaders) {
                parse_uniforms(path, uniforms);
            } for (auto& path: fragmentShaders) {
                parse_uniforms(path, uniforms);
            }

            // Then we actually compile the shaders
            program = PG_Unwrap(gl::makeShaderProgram(vertexShaders, fragmentShaders));
            // Finally, we can go through each of our uniforms and get their position
            for (auto& [name, loc] : uniforms) {
                // If we don't currently have a uniform for that name, find the uniform location
                if (uniforms.at(name)==-1) {
                    loc = glGetUniformLocation(program, name.c_str());
                }
            }
        }

    };

}
