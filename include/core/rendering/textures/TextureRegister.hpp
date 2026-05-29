#pragma once
#include <expected>
#include <toolkit/container-types/map_vector.hpp>
#include <toolkit/OpenGL/Texture.hpp>
#include <string_view>

#include "Image.hpp"
#include "errors/Error.hpp"

#include "stb_image/stb_image.h"

/* Created by Gordie Novak on 5/29/26.
 * Purpose: 
 * Stores and links textures */


namespace pg {

    class tex_id {
        uint32_t index;
        friend class TextureRegister;
        explicit constexpr tex_id(const uint32_t i) : index(i) {}
    public:
        constexpr tex_id() : index(0) {}
    };

    class TextureRegister {
    public:
        /** Creates a 2D texture from the given path and scale mode specified */
        [[nodiscard]] std::expected<tex_id, Error> create2D(std::string_view path, ScaleMode scale_mode = PG_LINEAR);

        /** Gets a specific texture at a given texture id */
        [[nodiscard]] std::expected<Texture*, Error> at(tex_id i);
        /** Gets a specific texture at a given path */
        [[nodiscard]] std::expected<Texture*, Error> at(std::string_view path) const;

        /** Unsafe: Gets texture given the texture index */
        [[nodiscard]] Texture& operator[](tex_id i);

        /// Binds the given texture to the current OpenGL context window.
        void bind(tex_id id, uint32_t textureSlot = GL_TEXTURE0);

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

        void init();
    };



}
