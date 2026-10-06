#pragma once

#include <string>
#include <toolkit/intdef.h>
#include <core/rendering/textures/tex_id.hpp>

namespace pg::err {

  struct TextureIndexOutOfBounds {
    u32 provided_id;
    u32 max_id;
    std::string str() const { return std::string("Texture index \"") + std::to_string(provided_id) + " out of bounds. (Max: " + std::to_string(max_id) + ")";}
  };

  struct TextureDestroyed {
    u32 provided_id;
    std::string str() const { return std::string("Texture \"") + std::to_string(provided_id) + "\" has already been destroyed."; }
  };

  struct NoTextureOnPath {
    std::string given_path;
    std::string str() const { return std::string("There is no texture created with the given path: ") + given_path; }
  };

}
