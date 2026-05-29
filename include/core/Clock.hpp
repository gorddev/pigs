#pragma once

#include "../toolkit/apidef.h"

namespace pg {
    /** Units for the timer are in milliseconds and frames. Please ignore the bad packing.
          * Timer is const because users should not overwrite any time parameters. */
    class Clock {
    private:
        friend class Engine;
        // private data members prevents random users from updating actual time values.
        uint64_t m_time;
        uint64_t m_dt;
        uint64_t m_frame;

        Clock();

        // Make sure the timer can't be moved/copied.
        Clock& operator=(Clock&&)        = default;   ///< Nope. You can't do that.
        Clock& operator=(const Clock&)   = default;   ///< Nope. You can't do that.
        Clock(Clock&&)                  = default;   ///< Nope. You can't do that.
        Clock(Clock&)                   = default;   ///< Nope. You can't do that.

        void tick();
    public:
        uint64_t time = 0;      ///< Total time since engine initialization in milliseconds.
        uint64_t dt = 0;          ///< Time since the previous frame
        uint64_t frame = 0;    ///< Current frame number


        friend class TimeUpdater;


    };
}