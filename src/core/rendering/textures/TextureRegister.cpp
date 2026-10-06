
#include <unwrap.hpp>
#include <core/rendering/OpenGL/Texture.hpp>
#include <core/rendering/textures/TextureRegister.hpp>
#include <core/rendering/textures/Image.hpp>

#include <err-types/engine/TextureRegisterErrors.hpp>
#include <err-types/library/STBImageError.hpp>

using namespace pg;
expected<tex_id, err::FileNotExists, err::STBImage, err::GenericOpenGLError,
         err::GLTexParameterInitFailure, err::GLTexGenerationFailure>
TextureRegister::create2D(const std::string_view path, ScaleMode scale_mode) {
  const auto t_path = std::string(path);
  if (const auto i = textures.indexOf(t_path); i) {
    return tex_id(i.value());
  }
  // Attempt to load the image
  auto img = Image::make(path);
  PG_ReturnIfUErr(img);

  auto tex = Texture::make2D(img->pixels, img->w, img->h, scale_mode);
  PG_ReturnIfUErr(tex);
  return tex_id(textures.add(t_path, std::move(*tex)));
}

expected<Texture *, err::TextureIndexOutOfBounds, err::TextureDestroyed>
TextureRegister::at(const tex_id i) {
  if (i.index > textures.size()) {
    return PG_UErrNew(err::TextureIndexOutOfBounds, .provided_id = i.index,
                      .max_id = textures.size());
  }
  if (textures.isDestroyed(i.index)) {
    return PG_UErrNew(err::TextureDestroyed, .provided_id = i.index);
  }
  return &textures[i.index];
}

expected<Texture *, err::NoTextureOnPath>
TextureRegister::at(std::string_view path) const {
  const auto str = std::string(path);
  if (auto opt = textures.map(str); opt) {
    return opt.value();
  }
  return PG_UErrNew(err::NoTextureOnPath, .given_path = std::string(path));
}

Texture &TextureRegister::operator[](tex_id i) { return textures[i.index]; }

void TextureRegister::glBind(const tex_id id, const uint32_t textureSlot) {
  glActiveTexture(textureSlot);
  glBindTexture(textures[id.index].target, (textures[id.index].gl_id));
}

void TextureRegister::destroyTexture(const tex_id id) {
  textures.removeWithIndex(id.index);
}

constexpr u8 tex_default_pixels[16] = {255, 0, 255, 255, 0,   0, 0,   0,
                                       0,   0, 0,   0,   255, 0, 255, 255};

void TextureRegister::init() {
	auto tex = PG_Unwrap(Texture::make2D(&tex_default_pixels, 2, 2, PG_PIXEL));
  textures.add("_default",
              std::move(tex));
}
