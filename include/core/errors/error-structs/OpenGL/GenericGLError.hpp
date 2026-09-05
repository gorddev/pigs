#pragma once

#include <string>
#include <toolkit/intdef.h>

namespace pg::err {

  struct GenericOpenGLError {
    u32 gl_error;

    std::string str() const {
      return std::string("OpenGL failed with error code: ") + std::to_string(gl_error);
    }
  };

}
