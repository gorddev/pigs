#pragma once
#include "scripting/Symbol.hpp"
#include "toolkit/serialization/concepts/is_specialization_of.hpp"
#include "toolkit/serialization/enumToString.hpp"
#include "toolkit/serialization/serialdef.hpp"
#include <meta>
#include <vector>
#include <string>

namespace pg::script {

  template<typename T>
  constexpr void reexpose(T& t, std::string_view display_str, std::vector<Symbol>& symbols) {

    static_assert(pg::ser::is_specialization_of_v<SettingVar, SettingVar<int>>);
    constexpr auto ctx = std::meta::access_context::unprivileged();
    template for(constexpr std::meta::info m : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, ctx))) {
      constexpr bool boop = pg::ser::is_array_like<decltype(t.[:m:])>
        || pg::ser::is_specialization_of_v<SettingVar, decltype(t.[:m:])>
        || std::is_fundamental_v<decltype(t.[:m:])>;
      std::string dstr(display_str);
      dstr = dstr + '.' + std::meta::identifier_of(m);
      auto& tm = t.[:m:];
      if constexpr(boop) {
        symbols.push_back(Symbol{dstr, tm});
      } else {
        reexpose(tm, dstr, symbols);
      }
    }
  }

  template<typename T>
  void expose(T& t, std::string_view display_str) {
    std::vector<Symbol> symbols;
    reexpose(t, display_str, symbols);
    for (auto& s: symbols) {
      std::cerr << s.name << ", " << ser::enumToString(s.type) << "\n";
    } std::cerr << std::flush;
  }
}
