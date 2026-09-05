#pragma once

/* Created by Gordie Novak on 8/14/26.
 * Purpose: 
 * Internal struct that manages whether certain stages in the engine have been met.*/

namespace pg {

    class Engine;
    class Config;

    inline class EngineStatus {
    private:
        template<typename T>
        class stat {
        private:
            T t{};
            stat(const T& t) : t(t) {}
            operator T&() { return t; }
            stat& operator=(const T& other) {
                t = other;
                return *this;
            }
            friend class Engine; friend class EngineStatus; friend class Config;
        public:
            operator const T&() const { return t; }
        };
    public:
        stat<bool> initialized  = false;
        stat<bool> in_main_loop = false;
    } engine_status;

}
