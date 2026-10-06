#pragma once

#include <core/rendering/RenderingSettings.hpp>
#include <toolkit/apidef.h>

#include <core/errors/err-types/OpenGL/GenericGLError.hpp>
#include <core/errors/err-types/OpenGL/GLTextureErrors.hpp>
#include <core/errors/unwrap.hpp>

/* Created by Gordie Novak on 3/11/26.
 * Purpose:
 * Creates & stores texture information into one convenient object.*/

namespace pg {

    using GLuint = uint32_t;

    /// Holds texture handle for a texture managed by OpenGL
    struct Texture {

        GLuint gl_id;   ///< Texture identifier GPU-side in OpenGL
        GLuint target;
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
         * @param texFormat The format of the texture in the GPU (Defaults to RGBA)
         * @param sizeofPixel The format of the pixels being uploaded (Defaults to GL_UNSIGNED_BYTE)
         * @return An optional texture object.
         * @warning The @code tex_id@endcode parameter is not initialized and must be done afterwords
         */
        static expected<
          Texture,
          err::GenericOpenGLError,
          err::GLTexParameterInitFailure,
          err::GLTexGenerationFailure>
          make2D(
            const void* pixels,
            const uint32_t w,
            const uint32_t h,
            const ScaleMode scaleMode,
            const GLenum internalFormat = GL_RGBA8, // e.g., GL_R8
            const GLenum pixelFormat = GL_RGBA,    // e.g., GL_RED
            const GLenum pixelType = GL_UNSIGNED_BYTE)      // e.g., GL_UNSIGNED_BYTE
          noexcept;


        /** @param pixels Raw pixels representing the image
         * @param w Width of the image
         * @param h Height of the image
         * @param scaleMode How the texture appears when scaled (linear vs. nearest neighbor)
         * @param target What kind of texture you would like to make.
         * @param packSize The alignment of the bytes you want to upload. (Defaults to 1)
         * @param texFormat The format of the texture in the GPU (Defaults to R8)
         * @param sizeofPixel The format of the pixels being uploaded (Defaults to GL_UNSIGNED_BYTE)
         * @return The texture object if creation was successful. @code std::nullopt@endcode otherwise.
         * Check @code PIG_GetLog()@endcode for error.
         */
        static expected<
          Texture,
          err::GLTexParameterInitFailure,
          err::GLTexGenerationFailure>
        make_packed(
            const void *pixels,
            u32 w,
            u32 h,
            ScaleMode scaleMode,
            GLuint target      = GL_TEXTURE_2D,
            GLuint packSize  = 1,
            GLuint texFormat = GL_R8,
            GLuint sizeofPixel = GL_UNSIGNED_BYTE
        ) noexcept;


        //TODO: Write definition
        static optional_err<
        	err::GLTexGenerationFailure>
        overwrite(
        	const void* pixels,
         	u32 start_x, u32 start_y,
          u32 end_x, u32 end_y,
          const GLenum pixelFormat = GL_RGBA,
          const GLenum pixelType = GL_UNSIGNED_BYTE
        );

        optional_err<
        	err::GLTexGenerationFailure>
        overwrite(
        	const void* pixels,
          const GLenum pixelFormat = GL_RGBA,
          const GLenum pixelType = GL_UNSIGNED_BYTE
        );

        Texture& glBind(GLuint textureSlot);

    private:
        Texture(GLuint, GLuint, uint32_t, uint32_t);
    };

}
