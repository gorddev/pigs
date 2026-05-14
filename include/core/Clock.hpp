#pragma once

#include "../toolkit/apidef.h"

namespace pig {
    /** Units for the timer are in milliseconds and frames. Please ignore the bad packing.
          * Timer is const because users should not overwrite any time parameters. */
    class Clock {
    private:
        // private data members prevents random users from updating actual time values.
        uint64_t m_time;
        uint64_t m_dt;
        uint64_t m_frame;

        Clock(uint64_t time, uint64_t dt, uint64_t frame);
    public:
        const uint64_t& time = m_time;      ///< Total time since engine initialization in milliseconds.
        const uint64_t& dt = m_dt;          ///< Time since the previous frame
        const uint64_t& frame = m_frame;    ///< Current frame number

        // Make sure the timer can't be moved/copied.
        Clock operator=(Clock&&)        = delete;   ///< Nope. You can't do that.
        Clock operator=(const Clock&)   = delete;   ///< Nope. You can't do that.
        Clock(Clock&&)                  = delete;   ///< Nope. You can't do that.
        Clock(Clock&)                   = delete;   ///< Nope. You can't do that.

        friend class TimeUpdater;

        static Clock create();

        static void tick(Clock& timer);
    };
}