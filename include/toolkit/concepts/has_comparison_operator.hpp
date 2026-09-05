#pragma once
#include <concepts>

namespace pg::concepts {

  template <typename T1, typename T2>
  concept has_comparison_op = requires(T1 a, T2 b) {
      { a < b } -> std::convertible_to<bool>;
      { a > b } -> std::convertible_to<bool>;
  };

  template<typename T1, typename T2>
  concept has_gt_op = requires(T1 a, T2 b) {
      { a > b } -> std::convertible_to<bool>;
  };

  template<typename T1, typename T2>
  concept has_lt_op = requires(T1 a, T2 b) {
      { a < b } -> std::convertible_to<bool>;
  };
}
