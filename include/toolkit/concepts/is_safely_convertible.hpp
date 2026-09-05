#include <type_traits>
#include <concepts>

namespace pg::concepts {

  template <typename From, typename To>
  struct is_safely_convertible_struct : std::false_type {};

  template <typename From, typename To>
    requires (std::is_arithmetic_v<From> && std::is_arithmetic_v<To>)
             || std::is_convertible_v<From, To>
  struct is_safely_convertible_struct<From, To> : std::true_type {};

  template <typename From, typename To>
  inline constexpr bool is_safely_convertible = is_safely_convertible_struct<From, To>::value;

}
