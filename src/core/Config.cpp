/* Created by Gordie Novak on 8/5/26.
 * Purpose:
 */
#include <Config.hpp>
#include <Engine.hpp>
#include <iostream>

#include "../../include/core/vitals/EngineStatus.hpp"
#include "errors/Error.hpp"
#include "errors/error-structs/engine/ModifyAssetsFolderAfterLoop.hpp"

namespace pg {

void Config::init([[maybe_unused]] Engine &e) {
  vsync.onModify([](const bool &b) {
    SDL_GL_SetSwapInterval(b);
    return SettingOpt::SET;
  });

  assets_folder.setting = '.';
  assets_folder.onModify([](const std::string &str) {
    if (engine_status.in_main_loop) {
      PG_ErrBurst(err::ModifyAssetsFolderAfterInit, .folder_name = str);
    } else {
      files::p_buffer.setBasePath(str);
    }
    return SettingOpt::SET;
  });

  audio.init(e);
}
} // namespace pg
