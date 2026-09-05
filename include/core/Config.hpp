#pragma once

#include <toolkit/containers/SettingVar.hpp>

#include "config/config_Audio.hpp"
#include "config/config_Warnings.hpp"
#include "config/config_Callback.hpp"
#include "config/config_Developer.hpp"
#include "toolkit/intdef.h"

namespace pg {

    using u32 = uint32_t;
    using u16 = uint16_t;
    using u8 = uint8_t;
    class Engine;

    /// Details all internal options the engine has access to.
    class Config {
        template<typename T> using sv = SettingVar<T>; using str = std::string;
    public:
        // All the variables that we can individually set.
        sv<bool> vsync             = true;      ///< Whether or not vsync is enabled
        sv<u32>  frame_rate        = 120ul;     ///< Target frame rate of the engine.
        sv<str>  assets_folder     = "assets";  ///< The default assets folder.

        config::Warnings      warn;             ///< Contains options for all warnings within the engine;
        config::AudioSettings audio;            ///< Contains options for audio.
        config::Callbacks     callback;         ///< Contains the callbacks for looping, events, quitting, and crashing.
        config::Developer     dev;              ///< Contains developer options.

    private:
        friend class Engine;
        void init(Engine& e);
    };

    namespace opt_nmspc {
        inline pg::Config pg_internal_options;
    }
}
