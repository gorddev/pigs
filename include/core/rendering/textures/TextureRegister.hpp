#pragma once
#include <expected>
#include <string>
#include <toolkit/containers/map_vector.hpp>
#include <core/rendering/OpenGL/Texture.hpp>
#include <rendering/textures/tex_id.hpp>
#include <string_view>

#include "Image.hpp"
#include "errors/error-structs/engine/TextureRegisterErrors.hpp"
#include "errors/unwrap.hpp"

#include "stb_image/stb_image.h"

/* Created by Gordie Novak on 5/29/26.
 * Purpose:
 * Stores and links textures */


namespace pg {

    class Engine;

    class TextureRegister {
    public:
        /** Creates a 2D texture from the given path and scale mode specified */
        [[nodiscard]] expected<
          tex_id,
          err::FileNotExists,
          err::STBImage,
          err::GenericOpenGLError,
          err::GLTexParameterInitFailure,
          err::GLTexGenerationFailure
        > create2D(std::string_view path, ScaleMode scale_mode = PG_LINEAR);

        /** Gets a specific texture at a given texture id */
        [[nodiscard]] expected<
          Texture*,
          err::TextureIndexOutOfBounds,
          err::TextureDestroyed>
        at(tex_id i);
        /** Gets a specific texture at a given path */
        [[nodiscard]] expected<
          Texture*,
          err::NoTextureOnPath>
        at(std::string_view path) const;

        /** Unsafe: Gets texture given the texture index */
        [[nodiscard]] Texture& operator[](tex_id i);

        /// Binds the given texture to the current OpenGL context window.
        void glBind(tex_id id, uint32_t textureSlot = GL_TEXTURE0);

        /** Destroys the given texture with the texture_id */
        void destroyTexture(tex_id id);

    private:
        friend class Engine;
        map_vector<std::string, Texture, uint32_t> textures{};

        TextureRegister() = default;

        TextureRegister& operator=(TextureRegister&& o) noexcept { std::swap(textures, o.textures); return *this; }
        TextureRegister(TextureRegister&& o) noexcept : textures(std::move(o.textures)) {}
        TextureRegister& operator=(const TextureRegister&) = delete;
        TextureRegister(const TextureRegister&) = delete;

        /// Uses OpenGL to compile a default texture and register it with the engine.
        void init();
    };
}
