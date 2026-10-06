#pragma once

#include <string>
#include <miniaudio/miniaudio.h>

namespace pg::err {

  struct MiniaudioInitFailure {
    const char* result_desc;
    std::string str() const {
      return std::string("Miniaudio failed initialization with result:\n") + result_desc;
    }
  };

  struct MiniaudioSoundInitFailure {
    const char* result_desc;
    std::string str() const {
      return std::string("Miniaudio failed to initialize sound with error:\n") + result_desc;
    }
  };

}
