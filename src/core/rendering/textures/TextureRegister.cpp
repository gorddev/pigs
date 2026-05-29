#include <core/rendering/textures/TextureRegister.hpp>

using namespace pg;
std::expected<tex_id, Error> TextureRegister::create2D(const std::string_view path, ScaleMode scale_mode) {
    const auto t_path = std::string(path);
    if (const auto i = textures.indexOf(t_path); i) {
        return tex_id(i.value());
    }

    // Attempt to load the image
    if (auto exp = Image::make(path.data()); exp) {
        // Attempt to load the texture
        if (auto tex = Texture::make2D(exp->pixels, exp->w, exp->h, scale_mode); tex) {
            // Happy path
            auto id =  tex_id(textures.add(t_path, std::move(*tex)));
            return id;
        } else {
            // Cannot create the texture
            return PG_CONTINUE_UERR(tex, "Unable to create texture from loaded image @", path);
        }
    } else {
        // Cannot load the image.
        return PG_CONTINUE_UERR(exp, "Unable to load image @", path);
    }
}

std::expected<Texture*, Error> TextureRegister::at(const tex_id i) {
    if (i.index > textures.size()) {
        return PG_MAKE_UERR("Texture index out of bounds.");
    } if (textures.isDestroyed(i.index)) {
        return PG_MAKE_UERR("Texture has already been destroyed");
    }
    return &textures[i.index];
}

std::expected<Texture*, Error> TextureRegister::at(std::string_view path) const {
    const auto str = std::string(path);
    if (auto opt = textures.map(str); opt) {
        return opt.value();
    }
    return PG_MAKE_UERR("No texture at the given path, ", path);
}

Texture& TextureRegister::operator[](tex_id i) {
    return textures[i.index];
}

void TextureRegister::bind(const tex_id id, const uint32_t textureSlot) {
    glActiveTexture(textureSlot);
    glBindTexture(textures[id.index].target, (textures[id.index].gl_id));
}

void TextureRegister::destroyTexture(const tex_id id) {
    textures.removeWithIndex(id.index);
}

void TextureRegister::init() {
    constexpr u8 pixels[4] = {255, 0, 255, 255};
    textures.add("_default", PG_UNWRAP(Texture::make2D(&pixels, 1, 1, PG_PIXEL)));
}