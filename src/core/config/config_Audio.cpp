#include <core/config/config_Audio.hpp>
#include <core/Engine.hpp>

void pg::config::AudioSettings::init(Engine& e) {

    master_volume.onModify([this, &e](const float& f) {
        auto* sg = e.audio.group<Master>();
        ma_sound_group_set_fade_in_milliseconds(sg, ma_sound_group_get_volume(sg), f, this->default_fade_length);
        return SettingOpt::SET;
    });

    music_volume.onModify([&e, this](const float&f) {
        [[maybe_unused]] auto* sg = e.audio.group<Music>();
        ma_sound_group_set_fade_in_milliseconds(sg, music_volume, f, this->default_fade_length);
        return SettingOpt::SET;
    });

    sfx_volume.onModify([this, &e](const float& f) {
        auto* sg = e.audio.group<SFX>();
        ma_sound_group_set_fade_in_milliseconds(sg, ma_sound_group_get_volume(sg), f, this->default_fade_length);
        return SettingOpt::SET;
    });

    vocal_volume.onModify([this, &e](const float& f) {
        auto* sg = e.audio.group<Vocal>();
        ma_sound_group_set_fade_in_milliseconds(sg, ma_sound_group_get_volume(sg), f, this->default_fade_length);
        return SettingOpt::SET;
    });
}
