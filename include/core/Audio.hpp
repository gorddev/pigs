#pragma once
#include "errors/unwrap.hpp"
#include <miniaudio/miniaudio.h>

/* Created by Gordie Novak on 8/13/26.
 * Purpose: 
 */


namespace pg {

    class Sound {
        friend class Sounds;
        uint16_t id;
        Sound() = default;
    };

    class Sounds {
    public:


        expected<Sound>

    private:
        Sounds() {
            if (const auto result = ma_engine_init(nullptr, &engine); result != MA_SUCCESS) {
                PG_Err("Failed to initialize miniaudio with error: ",
                                    ma_result_description(result)).burst();
            }
        };
        ma_engine engine;
    };

}
