#pragma once
#include <cstddef>

namespace pg::config {


/// Developer options for custom engine
struct Developer {
  bool no_loop = false;
  //std::nullptr_t panic_on_warning;
};

} // namespace pg::config
