#include "toolkit/OpenGL/Texture.hpp"

#include <expected>

#include "errors/Error.hpp"

using namespace pg;

Texture::~Texture() {
    glDeleteTextures(1, &gl_id);
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
    std::swap(w, o.w);
    std::swap(h, o.h);
    return *this;
}

Texture::Texture(const GLuint gl_id, const GLuint target, const uint32_t w, const uint32_t h)
    : gl_id(gl_id), target(target), w(w), h(h) {}

std::expected<Texture, Error> Texture::make2D(
    const void* pixels,
    const uint32_t w,
    const uint32_t h,
    const ScaleMode scaleMode,
    const GLuint texFormat, //< GL Macro for rgba
    const GLuint sizeofPixel)
    noexcept
{
    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        return PG_MAKE_UERR(
            "Texture before initialization failed.\nRefer to GL Error Code: " ,
            static_cast<int>(error) , "\n");
    }
    // first create a texture id.
    GLuint gl_id;
    // then create the texture in GL
    glGenTextures(1, &gl_id);
    // Bind the texture to the texture id.
    glBindTexture(GL_TEXTURE_2D, gl_id);


    if (scaleMode == PG_PIXEL) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else if (scaleMode == PG_LINEAR) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    if ((error = glGetError()) != GL_NO_ERROR) {
        return PG_MAKE_UERR(
            "Texture parameter initialization failed.\nRefer to GL Error Code: " ,
            static_cast<int>(error) , "\n");
    }

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


    if ((error = glGetError()) != GL_NO_ERROR) {
        return PG_MAKE_UERR(
            "Texture gener22ation failed.\nRefer to GL Error Code: ", static_cast<int>(error), "\n");
    }
    glGenerateMipmap(GL_TEXTURE_2D);

    if ((error = glGetError()) != GL_NO_ERROR) {
        return PG_MAKE_UERR(
            "Texture generation failed.\nRefer to GL Error Code: ", static_cast<int>(error), "\n");
    }
    // now we can return our texture id.
    return Texture{gl_id, GL_TEXTURE_2D, w, h};
}

std::optional<Texture> Texture::make_packed(const void* pixels, u32 w, u32 h, ScaleMode scaleMode,
    GLuint target, GLuint packSize, GLuint texFormat, GLuint sizeofPixel) noexcept
{
    // first create a texture id.
    GLuint gl_id;
    // then create the texture in GL
    glGenTextures(1, &gl_id);
    // Bind the texture to the texture id.
    glBindTexture(GL_TEXTURE_2D, gl_id);

    if (scaleMode == PG_PIXEL) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else if (scaleMode == PG_LINEAR) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        PIG_WriteLog("Texture::make()",
            "Texture parameter initialization failed.\nRefer to GL Error Code: " ,
            static_cast<int>(error) , "\n");
        return std::nullopt;
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
        PIG_WriteLog("Texture::make()",
            "Texture generation failed.\nRefer to GL Error Code: ", static_cast<int>(error), "\n");
        return std::nullopt;
    }
    // now we can return our texture id.
    return Texture{gl_id, target, w, h};
}
