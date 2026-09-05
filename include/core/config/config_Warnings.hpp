#pragma once
#include <type_traits>

#include "toolkit/serialization/serialdef.hpp"

namespace pg::config {



  /// Contains all warnings for the engine
  struct Warnings {
    #define wstruct(struct_name, member_vars) struct { member_vars } struct_name
    bool Werror = false; ///< Set to true if warnings panic the program.
    wstruct(init,
      bool no_event_func = true; 	///< Warns if there is no event function set.
      bool no_quit_func = true;		///< Warns if there is no quit function set.
      bool no_crash_func = true;	///< Warns if there is no crash function set.
    );
    wstruct(audio,
      bool missing_file = true;		///< Warns if there is a missing audio file.
      bool invalid_format = true;	///< Warns if a provided audio file is in an invalid format.
    );

    using static_layout = std::true_type;
    #undef wstruct
  };
  static_assert(pg::ser::is_statically_copyable<Warnings>);

} // namespace pg::config
