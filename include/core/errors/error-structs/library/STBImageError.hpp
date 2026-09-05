#pragma once

#include <string>
namespace pg::err {

  struct STBImage {
    std::string failure_reason;
    std::string str() const {
      return std::string("STB Image failed with reason:\n\"") +
        failure_reason + '"';
    }
  };
}
