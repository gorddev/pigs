#pragma once

#include <optional>
#include <toolkit/RenderingSettings.hpp>
#include <toolkit/errors/pig_log.hpp>

/* Created by Gordie Novak on 3/11/26.
 * Purpose: 
 * Creates & stores texture information into one convenient object.*/

namespace pig {

    using GLuint = uint32_t;
    using texID = uint32_t;


    /// Holds texture handle for a texture managed by OpenGL
    struct Texture {

        texID tex_id;   ///< Texture identifier CPU-side in the TextureRegister (texRg)
        GLuint gl_id;   ///< Texture identifier GPU-side in OpenGL
        u32 w, h;

        Texture() = default;
        ~Texture();

        Texture(const Texture&)             = delete;
        Texture& operator=(const Texture&)  = delete;

        Texture(Texture&&) noexcept;
        Texture& operator=(Texture&&) noexcept;

    /* ******************************************************************************************** */

        /** @param pixels Raw pixels representing the image
         * @param w Width of the image
         * @param h Height of the image
         * @param scaleMode How the texture appears when scaled (linear vs. nearest neighbor)
         * @param tex_id The position in the textureRegister vector of this particular texture.
         * @param texFormat The format of the texture in the GPU (Defaults to RGBA)
         * @param sizeofPixel The format of the pixels being uploaded (Defaults to GL_UNSIGNED_BYTE)
         * @return An optional texture object.
         * @warning The @code tex_id@endcode parameter is not initialized, and must be done afterwords
         */
        static std::optional<Texture> make(
            const void *pixels,
            u32 w,
            u32 h,
            ScaleMode scaleMode,
            texID tex_id,
            GLuint texFormat = GL_RGBA, //< GL Macro for rgba
            GLuint sizeofPixel = GL_UNSIGNED_BYTE
        ) noexcept;


        /** @param pixels Raw pixels representing the image
         * @param w Width of the image
         * @param h Height of the image
         * @param scaleMode How the texture appears when scaled (linear vs. nearest neighbor)
         * @param tex_id The position in the textureRegister vector of this particular texture.
         * @param packSize The alignment of the bytes you want to upload. (Defaults to 1)
         * @param texFormat The format of the texture in the GPU (Defaults to R8)
         * @param sizeofPixel The format of the pixels being uploaded (Defaults to GL_UNSIGNED_BYTE)
         * @return The texture object if creation was successful. @code std::nullopt@endcode otherwise.
         * Check @code GAN_GetLog()@endcode for error.
         */
        static std::optional<Texture> make_packed(
            const void *pixels,
            u32 w,
            u32 h,
            ScaleMode scaleMode,
            texID tex_id,
            GLuint packSize  = 1,
            GLuint texFormat = GL_R8,
            GLuint sizeofPixel = GL_UNSIGNED_BYTE
        ) noexcept;

    private:
        Texture(texID, GLuint, uint32_t, uint32_t);
    };

}
