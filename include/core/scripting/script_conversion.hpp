#pragma once

#include "errors/Error.hpp"
#include "toolkit/serialization/enumToString.hpp"
#include <toolkit/concepts/has_in_range.hpp>
#include "toolkit/serialization/serialdef.hpp"
#include <toolkit/concepts/is_safely_convertible.hpp>
#include <toolkit/concepts/has_comparison_operator.hpp>
#include <limits>
#include <core/Config.hpp>
#include <type_traits>
#include <iostream>
#include <utility>
namespace pg::script {

  #define PG_TypeSwitch_(type_enum, expr, default_case) \
      switch(type_enum) {   \
      case pg::ser::bin_t::U_BYTE: { \
        using Tm = u8;      \
        expr \
      } break;              \
      case pg::ser::bin_t::BYTE: {   \
        using Tm = i8; \
        expr \
      } break; \
      case pg::ser::bin_t::U_SHORT: { \
        using Tm = u16; \
        expr \
      } break; \
      case pg::ser::bin_t::SHORT: { \
        using Tm = i16; \
        expr \
      } break; \
      case pg::ser::bin_t::U_INT: { \
        using Tm = u32; \
        expr \
      } break; \
      case pg::ser::bin_t::INT: { \
        using Tm = i32; \
        expr \
      } break; \
      case pg::ser::bin_t::U_LONG: { \
        using Tm = u32; \
        expr \
      } break; \
      case pg::ser::bin_t::LONG: { \
        using Tm = i64; \
        expr \
      } break; \
      case pg::ser::bin_t::FLOAT32: { \
        using Tm = f32; \
        expr \
      } break; \
      case pg::ser::bin_t::FLOAT64: { \
        using Tm = f64; \
        expr \
      } break; \
      case pg::ser::bin_t::UNION: \
      case pg::ser::bin_t::TRIVIAL_STRUCT: \
      case pg::ser::bin_t::ARRAY_LIKE: \
      case pg::ser::bin_t::SETTING_VAR: \
      case pg::ser::bin_t::NAMED_STRUCT: \
      default: \
        default_case \
      }

  template <typename Target, typename Source>
  constexpr bool is_safe_integer_conversion(Source value) {
    if constexpr (std::is_integral_v<Target> && std::is_integral_v<Source> &&
                  !std::is_same_v<Target, bool> && !std::is_same_v<Source, bool>) {
      return std::in_range<Target>(value);
    } else if constexpr(concepts::has_comparison_op<Target, Source>){
        return value >= static_cast<Source>(std::numeric_limits<Target>::lowest()) &&
          value <= static_cast<Source>(std::numeric_limits<Target>::max());
    }
    return true;
  }

  template<typename T>
  [[nodiscard]] pg::Err isConvertibleTo(const T& t, ser::bin_t type) {
    static constexpr auto tname = enumToString(ser::typeToEnum<T>());
    PG_TypeSwitch_(type,
      if (!pg::concepts::is_safely_convertible<T, Tm>) {
        return PG_Err("Unable to convert from ", tname , "", " to ", enumToString(ser::typeToEnum<Tm>()));
      }
      if (!is_safe_integer_conversion<Tm>(t)) {
        return PG_Err("Value ", t , " is not within the range of ", enumToString(ser::typeToEnum<Tm>()), " {",
          std::numeric_limits<Tm>::min(), ", ", std::numeric_limits<Tm>::max(), "}");
      },
      PG_Panic("uh oh we're not supposed to be here.");
    )
    return pg::Err{};
  }
}
