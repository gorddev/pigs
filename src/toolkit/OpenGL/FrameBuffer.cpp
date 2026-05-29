#include "toolkit/OpenGL/FrameBuffer.hpp"
#include <limits>
#include <utility>

namespace pg {
    std::expected<FrameBuffer, Error> FrameBuffer::make(uint32_t w, uint32_t h, ScaleMode mode) noexcept {
        if (w == 0 || h == 0) {
            return PG_MAKE_UERR("Width and height parameters cannot be zero.");
        }
        if (w > std::numeric_limits<int16_t>::max() || h > std::numeric_limits<int16_t>::max()) {
            return PG_MAKE_UERR("Width and height parameters cannot be greater than ",
                std::numeric_limits<int16_t>::max());
        }

        FrameBuffer fb{};

        glGenFramebuffers(1, &fb.fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);

        // attempt to make a texture
        auto texOpt = Texture::make2D(nullptr, w, h, mode);
        if (!texOpt) {

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return PG_CONTINUE_UERR(texOpt, "Failed to create texture for framebuffer.");
        }
        fb.tex = std::move(texOpt.value());
        // attach texture to our bound framebuffer
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            fb.tex.gl_id,
            0
        );

        // now we create a depth buffer.
        glGenRenderbuffers(1, &fb.dbo);
        glBindRenderbuffer(GL_RENDERBUFFER, fb.dbo);
        // keeps track of fragment position
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            w, h // NOLINT(*-narrowing-conversions)
        );
        // actually attach the render buffer.
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            fb.dbo
        );

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            PG_MAKE_UERR(
                "Failed to create framebuffer as framebuffer is not marked as complete.\n"
                "OpenGL Error Code: ", glGetError());
        }

        fb.scaleMode = mode;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return fb;
    }

    bool FrameBuffer::resize(uint32_t w, uint32_t h) noexcept {
        if (w == 0 || h == 0) {
            PIG_WriteLog("FrameBuffer::resize()", "Width and height parameters cannot be zero.");
            return false;
        }
        if (w > std::numeric_limits<int16_t>::max() || h > std::numeric_limits<int16_t>::max()) {
            PIG_WriteLog("FrameBuffer::resize()", "Width and height parameters exceed limits.");
            return false;
        }

        // 1. Resize the texture storage
        glBindTexture(GL_TEXTURE_2D, this->tex.gl_id);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA,
            w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );

        this->tex.w = w;
        this->tex.h = h;

        glBindRenderbuffer(GL_RENDERBUFFER, this->dbo);
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            w, h
        );

        glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            PIG_WriteLog("FrameBuffer::resize()", "Framebuffer became incomplete after resize.");
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return true;
    }

    void FrameBuffer::bind() const noexcept {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, tex.w, tex.h);
    }

    /** @warning Does not bind the frameBuffer to blit to initially.  */
    void FrameBuffer::blit(const rect& dst) const {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBlitFramebuffer(
            0, 0, tex.w, tex.h, //< src
            dst.x, dst.y, dst.x + dst.w, dst.y + dst.h, //< dst
            GL_COLOR_BUFFER_BIT, scaleMode
        );
    }

    /** Draws the framebuffer to the currently bound target */
    void FrameBuffer::draw(const frect& dst) const {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex.gl_id);

        const PVertex vertices[4] = {
            {dst.x,          dst.y,          0.0f}, // Top-left
            {dst.x + dst.w,  dst.y,          0.0f}, // Top-right
            {dst.x,          dst.y + dst.h,  0.0f}, // Bottom-left
            {dst.x + dst.w,  dst.y + dst.h,  1.0f}  // Bottom-right
        };

        const auto vb = VBuffer<PVertex>::make(vertices, 4);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->tex.gl_id);

        glBindBuffer(GL_ARRAY_BUFFER, vb.vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_DYNAMIC_DRAW
        );

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    FrameBuffer::~FrameBuffer() {
        glDeleteRenderbuffers(1, &dbo);
        glDeleteFramebuffers(1, &fbo);
    }

    FrameBuffer& FrameBuffer::operator=(FrameBuffer&& o) noexcept {
        std::swap(this->tex, o.tex);
        std::swap(this->scaleMode, o.scaleMode);
        std::swap(this->fbo, o.fbo);
        std::swap(this->dbo, o.dbo);
        return *this;
    }

    FrameBuffer::FrameBuffer(FrameBuffer&& o) noexcept
        : fbo(o.fbo), dbo(o.dbo), tex(std::move(o.tex)), scaleMode(o.scaleMode)
    {
        o.fbo = 0;
        o.dbo = 0;
    }

}
