#include <core/Clock.hpp>

namespace pig {
    Clock::Clock(uint64_t time, uint64_t dt, uint64_t frame) :
        m_time(time), m_dt(dt), m_frame(frame) {}

    Clock Clock::create() {
        return {SDL_GetTicks(), 0, 0ull};
    }

    void Clock::tick(Clock& timer) {
        const uint64_t newTime = SDL_GetTicks();
        timer.m_dt = newTime - timer.m_time;
        timer.m_time = newTime;
        ++timer.m_frame;
    }
}
