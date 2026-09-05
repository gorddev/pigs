#pragma once

#include <toolkit/containers/SettingVar.hpp>

#include "toolkit/typedef.h"

namespace pg {

    using u32 = uint32_t;
    using u16 = uint16_t;
    using u8 = uint8_t;
    class Engine;

    /// Details all internal options the engine has access to.
    class Options {
        template<typename T> using sv = SettingVar<T>; using str = std::string;
    public:
        // All the variables that we can individually set.
        sv<bool> vsync             = true;
        sv<u32>  frame_rate        = 120ul;
        sv<str>  assets_folder     = "assets";      ///< The default assets folder.

        struct Warnings {
            struct {
                bool no_event_func = true;
                bool no_quit_func  = true;
                bool no_crash_func = true;
            } init;
            struct {
                bool missing_file = true;
                bool invalid_format = true;
            } audio;
            using static_layout = std::true_type;
        } warn;

        struct AudioSettings {
            sv<float> master_volume = 1.f;
            sv<float> music_volume  = 1.f;
            sv<float> sfx_volume    = 1.f;
            sv<float> vocal_volume  = 1.f;
            u64 default_fade_length = 300;
        } audio;

    private:
        friend class Engine;
        void load_options(Engine& e);
    };

    namespace opt_nmspc {
        inline pg::Options pg_internal_options;
    }
}
