#include <core/Clock.hpp>

namespace pg {
    Clock::Clock() :
        m_time(0), m_dt(0), m_frame(0) {}

    void Clock::tick() {
        const uint64_t newTime = SDL_GetTicks();
        m_dt = newTime - m_time;
        m_time = newTime;
        ++m_frame;

        time = m_time;
        frame = m_frame;
        dt = m_dt;
    }
}
