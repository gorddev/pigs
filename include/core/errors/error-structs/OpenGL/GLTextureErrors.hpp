#pragma once

#include <string>
#include <toolkit/intdef.h>

namespace pg::err {

  struct GLTexParameterInitFailure {
    u32 gl_error;
    std::string str() const {
      return std::string("OpenGL failed to initialize texture parameters with error code: ") + std::to_string(gl_error);
    }
  };

  struct GLTexGenerationFailure {
    u32 gl_error;
    std::string str() const {
      return std::string("OpenGL failed to generate texture with error code: ") + std::to_string(gl_error);
    }
  };

}
