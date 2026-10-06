#pragma once

#include <optional>

#include "Texture.hpp"
#include <core/errors/err-types/OpenGL/GLFramebufferErrors.hpp>
#include <core/errors/err-types/OpenGL/GLTextureErrors.hpp>
#include <core/errors/err-types/OpenGL/GenericGLError.hpp>
#include <toolkit/types/rect.hpp>
#include <toolkit/types/frect.hpp>


/* Created by Gordie Novak on 3/13/26.
 * Purpose:
 * Stores a writable texture (a framebuffer) so for each scene, or shadow obj,
 * you can write to a framebuffer, and then scale it up to the window!
 */

namespace pg {
    struct VertexBuffer;

    class FrameBuffer {
    public:
        GLuint fbo;     ///< Handle to the GL Framebuffer
        GLuint dbo;     ///< Handle to the depth buffer for depth testing.
        Texture tex;    ///< Texture we write to with the FrameBuffer
        ScaleMode scaleMode;

        FrameBuffer() = default;
        FrameBuffer& operator=(const FrameBuffer&) = delete;
        FrameBuffer(const FrameBuffer&) = delete;

    /* ******************************************************************************************** */

        /**
         * @param w The width of the framebuffer you want to generate
         * @param h The height of the framebuffer you want to generate
         * @param mode The scale mode of the framebuffer generated
         * @return A Framebuffer if creation was successful. An @code std::nullopt@endcode otherwise. */
        static pg::expected<
        	FrameBuffer,
         	err::GLTexGenerationFailure,
          err::GLTexParameterInitFailure,
          err::GenericOpenGLError,
          err::GLFramebufferIncomplete>
        make(uint32_t w, uint32_t h, ScaleMode mode = PG_PIXEL) noexcept;

        optional_err<err::GLFramebufferInvalidResize> resize(uint32_t w, uint32_t h) noexcept;

        /** Bind this framebuffer to be drawn to */
        void bind() const noexcept;

        /** Draws this framebuffer onto the current target framebuffer.  */
        void blit(const rect& dst) const;

        /** Draws the framebuffer to the currently bound target */
        void draw(const frect& dst) const;

        ~FrameBuffer();

        FrameBuffer& operator=(FrameBuffer&& o) noexcept;
        FrameBuffer(FrameBuffer&& o) noexcept;
    };

}
