#pragma once
#include <cstdint>
#include <limits>

namespace pg {

    // Internal Engine Types
    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using f32 = float;
    using f64 = double;

    // GL Types
    using GLint  = int;
    using GLuint = u32;
    using GLenum = u32;

    // Minimums and maximums
    constexpr auto u8_MIN  = std::numeric_limits<u8>::min();
    constexpr auto u8_MAX  = std::numeric_limits<u8>::max();
    constexpr auto u16_MIN = std::numeric_limits<u16>::min();
    constexpr auto u16_MAX = std::numeric_limits<u16>::max();
    constexpr auto u32_MIN = std::numeric_limits<u32>::min();
    constexpr auto u32_MAX = std::numeric_limits<u32>::max();
    constexpr auto u64_MIN = std::numeric_limits<u64>::min();
    constexpr auto u64_MAX = std::numeric_limits<u64>::max();

    constexpr auto i8_MIN  = std::numeric_limits<i8>::min();
    constexpr auto i8_MAX  = std::numeric_limits<i8>::max();
    constexpr auto i16_MIN = std::numeric_limits<i16>::min();
    constexpr auto i16_MAX = std::numeric_limits<i16>::max();
    constexpr auto i32_MIN = std::numeric_limits<i32>::min();
    constexpr auto i32_MAX = std::numeric_limits<i32>::max();
    constexpr auto i64_MIN = std::numeric_limits<i64>::min();
    constexpr auto i64_MAX = std::numeric_limits<i64>::max();

    constexpr auto f32_MIN  = std::numeric_limits<f32>::min();
    constexpr auto f32_MAX  = std::numeric_limits<f32>::max();
    constexpr auto f64_MIN  = std::numeric_limits<f64>::min();
    constexpr auto f64_MAX  = std::numeric_limits<f64>::max();
}
