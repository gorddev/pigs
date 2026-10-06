#include "core/rendering/OpenGL/Texture.hpp"
#include "err-types/OpenGL/GenericGLError.hpp"
#include "toolkit/apidef.h"

#include <expected>

#include <unwrap.hpp>

using namespace pg;

Texture::~Texture() {
	if (gl_id) {
    glDeleteTextures(1, &gl_id);
	}
}

Texture::Texture(Texture&& o) noexcept {
    gl_id = o.gl_id;
    target = o.target;
    o.gl_id = 0;
    w = o.w;
    h = o.h;
}

Texture& Texture::operator=(Texture&& o) noexcept {
    std::swap(gl_id, o.gl_id);
    std::swap(target, o.target);
    std::swap(w, o.w);
    std::swap(h, o.h);
    return *this;
}

Texture::Texture(const GLuint gl_id, const GLuint target, const uint32_t w, const uint32_t h)
    : gl_id(gl_id), target(target), w(w), h(h) {}

expected<
    Texture,
    err::GenericOpenGLError,
    err::GLTexParameterInitFailure,
    err::GLTexGenerationFailure>
    Texture::make2D(
        const void* pixels,
        const uint32_t w,
        const uint32_t h,
        const ScaleMode scaleMode,
        const GLenum internalFormat, // e.g., GL_R8
        const GLenum pixelFormat,    // e.g., GL_RED
        const GLenum pixelType)      // e.g., GL_UNSIGNED_BYTE
    noexcept
{
    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        return PG_UErrNew(err::GenericOpenGLError, .gl_error = error);
    }
    // first create a texture id.
    GLuint gl_id;
    // then create the texture in GL
    glGenTextures(1, &gl_id);
    // Bind the texture to the texture id.
    glBindTexture(GL_TEXTURE_2D, gl_id);


    if (scaleMode == PG_PIXEL) {
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else if (scaleMode == PG_LINEAR) {
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    if ((error = glGetError()) != GL_NO_ERROR) {
        return PG_UErrNew(err::GLTexParameterInitFailure, .gl_error = error);
    }

    // then we generate the underlying texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        static_cast<int>(internalFormat),
        static_cast<int>(w), static_cast<int>(h),
        0,
        pixelFormat,
        pixelType,
        pixels
    );


    if ((error = glGetError()) != GL_NO_ERROR) {
        return PG_UErrNew(err::GLTexGenerationFailure, .gl_error = error);
    }
    glGenerateMipmap(GL_TEXTURE_2D);

    if ((error = glGetError()) != GL_NO_ERROR) {
      return PG_UErrNew(err::GLTexGenerationFailure, .gl_error = error);
    }
    // now we can return our texture id.
    return Texture{gl_id, GL_TEXTURE_2D, w, h};
}

expected<
  Texture,
  err::GLTexParameterInitFailure,
  err::GLTexGenerationFailure>
Texture::make_packed(const void* pixels, u32 w, u32 h, ScaleMode scaleMode,
    GLuint target, [[maybe_unused]]GLuint packSize, GLuint texFormat, GLuint sizeofPixel) noexcept
{
    // first create a texture id.
    GLuint gl_id;
    // then create the texture in GL
    glGenTextures(1, &gl_id);
    // Bind the texture to the texture id.
    glBindTexture(GL_TEXTURE_2D, gl_id);


    if (scaleMode == PG_PIXEL) {
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else if (scaleMode == PG_LINEAR) {
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        return PG_UErrNew(err::GLTexParameterInitFailure, .gl_error = error);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // then we generate the underlying texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        static_cast<int>(texFormat),
        static_cast<int>(w), static_cast<int>(h),
        0,
        texFormat,
        sizeofPixel,
        pixels
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    if ((error = glGetError()) != GL_NO_ERROR) {
        return PG_UErrNew(err::GLTexGenerationFailure, .gl_error = error);
    }
    // now we can return our texture id.
    return Texture{gl_id, target, w, h};
}


Texture& Texture::glBind(GLuint textureSlot) {
  glActiveTexture(textureSlot);
  glBindTexture(target, gl_id);
  return *this;
}


optional_err<
	err::GLTexGenerationFailure>
Texture::overwrite(
	const void* pixels,
  const GLenum pixelFormat,
  const GLenum pixelType
) {
	glBindTexture(this->target, this->gl_id);
	glTexSubImage2D(
		target,
		0,
		0, 0,
		this->w, this->h,
		pixelFormat,
		pixelType,
		pixels
	);
	glBindTexture(this->target, 0);

	return std::nullopt;
}
