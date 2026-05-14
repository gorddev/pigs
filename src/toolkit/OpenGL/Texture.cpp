#include "toolkit/OpenGL/Texture.hpp"

using namespace pig;

Texture::~Texture() {
    glDeleteTextures(1, &gl_id);
}

Texture::Texture(Texture&& o) noexcept {
    tex_id = o.tex_id;
    o.tex_id = 0;
    gl_id = o.gl_id;
    o.gl_id = 0;
    w = o.w;
    h = o.h;
}

Texture& Texture::operator=(Texture&& o) noexcept {
    std::swap(tex_id, o.tex_id);
    std::swap(gl_id, o.gl_id);
    std::swap(w, o.w);
    std::swap(h, o.h);
    return *this;
}

Texture::Texture(const texID id, const GLuint gl_id, const uint32_t w, const uint32_t h)
    : tex_id(id), gl_id(gl_id), w(w), h(h) {}


std::optional<Texture> Texture::make(
            const void *pixels,
            const uint32_t w,
            const uint32_t h,
            const ScaleMode scaleMode,
            const texID tex_id,
            const GLuint texFormat, //< GL Macro for rgba
            const GLuint sizeofPixel)
noexcept {
    // first create a texture id.
    GLuint gl_id;
    // then create the texture in GL
    glGenTextures(1, &gl_id);
    // Bind the texture to the texture id.
    glBindTexture(GL_TEXTURE_2D, gl_id);

    if (scaleMode == GAN_PIXEL) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else if (scaleMode == GAN_LINEAR) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        GAN_WriteLog("Texture::make()",
            "Texture parameter initialization failed.\nRefer to GL Error Code: " ,
            static_cast<int>(error) , "\n");
        return std::nullopt;
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
    glGenerateMipmap(GL_TEXTURE_2D);

    if ((error = glGetError()) != GL_NO_ERROR) {
        GAN_WriteLog("Texture::make()",
            "Texture generation failed.\nRefer to GL Error Code: ", static_cast<int>(error), "\n");
        return std::nullopt;
    }
    // now we can return our texture id.
    return Texture{tex_id, gl_id, w, h};
}

std::optional<Texture> Texture::make_packed(const void* pixels, u32 w, u32 h, ScaleMode scaleMode, texID tex_id,
    GLuint packSize, GLuint texFormat, GLuint sizeofPixel) noexcept
{
    // first create a texture id.
    GLuint gl_id;
    // then create the texture in GL
    glGenTextures(1, &gl_id);
    // Bind the texture to the texture id.
    glBindTexture(GL_TEXTURE_2D, gl_id);

    if (scaleMode == GAN_PIXEL) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else if (scaleMode == GAN_LINEAR) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    GLenum error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        GAN_WriteLog("Texture::make()",
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
        GAN_WriteLog("Texture::make()",
            "Texture generation failed.\nRefer to GL Error Code: ", static_cast<int>(error), "\n");
        return std::nullopt;
    }
    // now we can return our texture id.
    return Texture{tex_id, gl_id, w, h};
}
