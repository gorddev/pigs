/* Created by Gordie Novak on 8/19/26.
 * Purpose:
 */

#include <core/Audio.hpp>
#include <ranges>

#include <core/Config.hpp>
#include <Error.hpp>
#include <err-types/library/miniaudioError.hpp>

using namespace pg;

void Audio::start_sound(const Sound s) {
  ma_sound_start(sounds.find(s.id)->second.get());
}

i32 Audio::init_sfx_nt(const path &file) {
  if (!std::filesystem::exists(file)) {
    PG_Warn_(audio.missing_file, "Audio File \"", file, "\" does not exist.");
    return -1;
  }
  const std::string file_str(file);
  if (initialized_files.contains(file_str)) {
    return initialized_files[file_str];
  }

  auto sound_ptr = std::make_unique<ma_sound>();
  auto sound = sound_ptr.get();
  sounds.emplace(++hasher, std::move(sound_ptr));
  const ma_result result =
      ma_sound_init_from_file(&engine, file, 0, nullptr, nullptr, sound);
  if (result != MA_SUCCESS) {
    PG_Warn_(audio.invalid_format, "File '", file,
             "' passed to audio engine with error: \"",
             ma_result_description(result), '"');
    sounds.erase(hasher--);
    return -1;
  }
  initialized_files[file_str] = sounds.size();
  return static_cast<u16>(sounds.size());
};

Audio::~Audio() {
  struct empty {};
  for (auto &sound : sounds | std::views::values) {
    ma_sound_uninit(sound.get());
  }
  ma_engine_uninit(&engine);
}

Audio::Audio()
    : master(std::make_unique<ma_sound_group>()),
      music(std::make_unique<ma_sound_group>()),
      sfx(std::make_unique<ma_sound_group>()),
      vocal(std::make_unique<ma_sound_group>()), hasher(0) {
  if (const auto result = ma_engine_init(nullptr, &engine);
      result != MA_SUCCESS) {
    PG_ErrBurst(err::MiniaudioInitFailure,
                .result_desc = ma_result_description(result));
  }
  // Initialize all of our sound groups.
  ma_sound_group_init(&engine, 0, nullptr, master.get());
  ma_sound_group_init(&engine, 0, master.get(), music.get());
  ma_sound_group_init(&engine, 0, master.get(), sfx.get());
  ma_sound_group_init(&engine, 0, master.get(), vocal.get());
}
