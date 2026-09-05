#pragma once

namespace pg::platform {

    enum Architecture {
        X86_32,
        X86_64,
        ARM64,
        ARM32,
        UNKNOWN
    };

    // First we go through each of
    #if defined(_WIN32)
    	constexpr bool win32 = true;
    	constexpr char name[] = "Windows"
    #else
    	constexpr bool win32 = false;
    #endif
    #if defined(__APPLE__)
    	constexpr bool apple = true;
    	constexpr char name[] = "MacOS";
    #else
    	constexpr bool apple = false;
    #endif
    #if defined(__linux__)
    	constexpr bool linux = true;
    	constexpr char name[] = "linux";
    #else
    	constexpr bool linux = false;
    #endif
    #if defined(__EMSCRIPTEN__)
    	constexpr char name[] = "Emscripten";
    	constexpr bool emscripten = true;
    #else
    	constexpr bool emscripten = false;
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
    	constexpr Architecture architecture = pg::platform::X86_64;
    	constexpr char architecture_str [] = "x86_64";
    #elif defined(__i386__) || defined(_M_IX86)
    	constexpr Architecture architecture = pg::platform::X86_32;
    	constexpr char architecture_str [] = "x86_32";
    #elif defined(__aarch64__) || defined(_M_ARM64)
    	constexpr Architecture architecture = ARM64;
    	constexpr char architecture_str [] = "ARM64";
    #elif defined(__arm__) || defined(_M_ARM)
    	constexpr Architecture architecture = ARM32;
    	constexpr char architecture_str [] = "ARM32";
    #else
    	constexpr Architecture architecture = UNKNOWN;
    	constexpr char architecture_str [] = "Architecture Unknown";
    #endif
}
