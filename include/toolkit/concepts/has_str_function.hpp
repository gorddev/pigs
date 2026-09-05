#pragma once

#include <type_traits>

namespace pg::concepts {

  template <typename T>
  concept has_str_method = requires(const T t) {
      { t.str() } -> std::convertible_to<std::string>;
  };

  template <typename... Args>
  concept all_have_str_method = (has_str_method<std::remove_cvref_t<Args>> && ...);

}
