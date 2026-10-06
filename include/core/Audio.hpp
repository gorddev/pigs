#pragma once
#include <future>

#include <core/errors/unwrap.hpp>
#include <miniaudio/miniaudio.h>

#include "ankerl-hash-map/dense_map.hpp"
#include "core/filesystem/path.hpp"

/* Created by Gordie Novak on 8/13/26.
 * Purpose:
 */


namespace pg {

    class Sound {
        friend class Audio;
        i32 id;
        explicit Sound(const i32 i) : id(i){};
    public:
        Sound() = default;
    };

    struct Master {};
    struct SFX {};
    struct Music {};
    struct Vocal {};

    namespace aio {
        template<typename T>
        concept is_default_group =
            std::is_same_v<T, Master> ||
            std::is_same_v<T, Music> ||
            std::is_same_v<T, Vocal> ||
            std::is_same_v<T, SFX>;
    }

    class Audio {

    public:

        void start_sound(Sound s);

        template<typename T>
            requires(aio::is_default_group<T>)
        [[nodiscard]] Sound init_sfx(const path& file);

        template<typename T>
            requires (aio::is_default_group<T>)
        constexpr ma_sound_group* group() const;

        ~Audio();

    private:

        friend class Engine;
        Audio();

        [[nodiscard]] i32 init_sfx_nt(const path& file);



        /// Audio engine object used by audio class
        ma_engine engine{};
        ankerl::unordered_dense::map<std::string, i32> initialized_files;
        ankerl::unordered_dense::map<i32, std::unique_ptr<ma_sound>> sounds;

        std::unique_ptr<ma_sound_group> master;
        std::unique_ptr<ma_sound_group> music;
        std::unique_ptr<ma_sound_group> sfx;
        std::unique_ptr<ma_sound_group> vocal;

        i32 hasher;
    };

    template<typename T>
        requires(aio::is_default_group<T>)
    Sound Audio::init_sfx(const path& file) {
        const i32 s = init_sfx_nt(file);
        if (s > 0) {
            ma_node_attach_output_bus(sounds[s].get(), 0, group<T>(), 0);
        }
        return Sound{s};
    }

    template<typename T>
    requires (aio::is_default_group<T>)
    constexpr ma_sound_group* Audio::group() const {
        if constexpr(std::is_same_v<T, SFX>) {
            return sfx.get();
        } else if constexpr(std::is_same_v<T, Music>) {
            return music.get();
        } else if constexpr(std::is_same_v<T, Vocal>) {
            return vocal.get();
        } else if constexpr(std::is_same_v<T, Master>) {
            return master.get();
        }
        return nullptr;
    }
}
