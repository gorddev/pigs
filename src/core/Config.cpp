/* Created by Gordie Novak on 8/5/26.
 * Purpose:
 */
#include <core/Config.hpp>
#include <core/Engine.hpp>

#include "../../include/core/vitals/EngineStatus.hpp"
#include "SDL3/SDL_video.h"
#include <Error.hpp>
#include <err-types/engine/ModifyAssetsFolderAfterLoop.hpp>

namespace pg {

void Config::init([[maybe_unused]] Engine &e) {
  vsync.onModify([](const bool &b) {
	  if (b && !SDL_GL_SetSwapInterval(-1)) {
	      SDL_GL_SetSwapInterval(1);
	  } else {
				SDL_GL_SetSwapInterval(0);
			}
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
