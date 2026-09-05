#pragma once
#include <string>

#include "toolkit/containers/SettingVar.hpp"
#include "toolkit/intdef.h"

/* Created by Gordie Novak on 8/19/26.
 * Purpose:
 */

namespace pg {
class Engine;
}

namespace pg::config {
/// Conatins all audio settings that are part of the engine.
struct AudioSettings {
  template <typename T> using sv = SettingVar<T>;
  using str = std::string;
  sv<float> master_volume = 1.f;
  sv<float> music_volume = 1.f;
  sv<float> sfx_volume = 1.f;
  sv<float> vocal_volume = 1.f;
  sv<u64> default_fade_length = 200;

  void init(Engine &e);
};

} // namespace pg::config
