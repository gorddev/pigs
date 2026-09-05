#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <core/rendering/textures/tex_id.hpp>
#include <core/rendering/textures/TextureRegister.hpp>
#include <core/rendering/shaders/ShaderRef.hpp>

namespace pg {

    enum class BlendMode { Opaque, Transparent, Additive };
    enum class CullMode { Back, Front, None };

    struct TextureSlot {
        std::string uniformName;
        tex_id texture;
    };

    class Material {
        mutable std::unordered_map<std::string, GLint> uniformLocationCache;

        GLint getUniformLocation(const std::string& uniformName) const {
            if (auto it = uniformLocationCache.find(uniformName); it != uniformLocationCache.end()) {
                return it->second;
            }
            GLint location = glGetUniformLocation(shader.program, uniformName.c_str());
            uniformLocationCache[uniformName] = location;
            return location;
        }

    public:
        // Core Identity
        std::string name;
        ShaderRef shader{0};

        std::unordered_map<std::string, float>     floats;
        std::unordered_map<std::string, int>       ints;
        std::unordered_map<std::string, glm::vec3> vec3s;
        std::unordered_map<std::string, glm::vec4> vec4s;
        std::unordered_map<std::string, glm::mat4> mat4s;
        std::vector<TextureSlot>                  textures;

        // Pipeline States
        BlendMode blendMode = BlendMode::Opaque;
        CullMode  cullMode  = CullMode::Back;
        bool      depthTest = true;
        bool      depthWrite = true;

        // Called right before drawing an object
        void bind(TextureRegister& texRegister) const {
            // 1. Activate shader
            glUseProgram(shader.program);

            // 2. Upload uniform data
            for (const auto& [key_name,  val] : floats) {
                glUniform1f(getUniformLocation(key_name), val);
            }
            for (const auto& [key_name,  val] : ints) {
                glUniform1i(getUniformLocation(key_name), val);
            }
            for (const auto& [key_name,  val] : vec3s) {
                glUniform3fv(getUniformLocation(key_name), 1, &val[0]);
            }
            for (const auto& [key_name,  val] : vec4s) {
                glUniform4fv(getUniformLocation(key_name), 1, &val[0]);
            }
            for (const auto& [key_name,  val] : mat4s) {
                glUniformMatrix4fv(getUniformLocation(key_name), 1, GL_FALSE, &val[0][0]);
            }

            // 3. Bind Textures
            for (uint32_t i = 0; i < textures.size(); ++i) {
                const auto& slot = textures[i];
                texRegister.glBind(slot.texture, GL_TEXTURE0 + i);
                glUniform1i(getUniformLocation(slot.uniformName), i);
            }

            // 4. Configure OpenGL Hardware State
            if (depthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
            glDepthMask(depthWrite ? GL_TRUE : GL_FALSE);

            if (cullMode == CullMode::Back) {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            } else if (cullMode == CullMode::Front) {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
            } else {
                glDisable(GL_CULL_FACE);
            }

            if (blendMode == BlendMode::Transparent) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            } else if (blendMode == BlendMode::Additive) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            } else {
                glDisable(GL_BLEND);
            }
        }
    };

}
