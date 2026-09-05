#include <utility>
#include <concepts>

namespace pg::concepts {

  template <typename Target, typename Source>
  concept has_std_in_range = requires(Source s) {
      { std::in_range<Target>(s) } -> std::same_as<bool>;
  };

}
