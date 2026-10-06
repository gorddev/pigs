
#include <SDL3/SDL.h>
#include <core/Clock.hpp>
#include <core/Config.hpp>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/**
 * High-precision cross-platform delay.
 * Works with native OS schedulers on Desktop and Asyncify on Web Assembly.
 */
[[maybe_unused]] inline void delayLoopUntil(pg::Clock& c, pg::Config& conf) {
    uint64_t remaining_ns = SDL_GetTicksNS() - c.time;
    uint64_t target = 1000000000.0/conf.frame_rate;

    if (remaining_ns >= target) {
        return;
    }

    if (remaining_ns > 2500000) { // 2.5ms threshold
        #ifdef __EMSCRIPTEN__
        double sleep_ms = (double)(remaining_ns - 2500000) / 1000000.0;
        emscripten_sleep((unsigned int)sleep_ms);
        #else
        Uint32 sleep_ms = (Uint32)((remaining_ns - 2500000) / 1000000);
        SDL_Delay(sleep_ms);
        #endif
    }

    // ==========================================
    // PHASE 2: FINE SPIN LOCK (Microsecond Snap)
    // ==========================================
    while (SDL_GetTicksNS() < target) {
        #ifndef __EMSCRIPTEN__
        // Desktop ONLY: Inline assembly/intrinsics to tell the CPU to optimize power
        #if defined(__x86_64__) || defined(_M_X64)
            #if defined(_MSC_VER)
                __inlinevoid__mm_pause();
            #else
                __builtin_ia32_pause();
            #endif
        #elif defined(__arm__) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64)
            #if defined(_MSC_VER)
                __yield();
            #else
                __asm__ __volatile__("yield" ::: "memory");
            #endif
        #endif
        #else

        #endif
    }
}
