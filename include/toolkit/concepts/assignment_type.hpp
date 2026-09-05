#pragma once

#include <toolkit/containers/SettingVar.hpp>
namespace pg::concepts {

  template<typename T>
    struct get_assignment_type_struct {
    using value_type = T;
  };

  template<typename T>
  struct get_assignment_type_struct<SettingVar<T>> {
    using value_type = T;
  };

  template<typename T>
  using assignment_type = get_assignment_type_struct<T>::value_type;

  static_assert(std::is_same_v<assignment_type<SettingVar<int>>, int>);

}
