#include <panic.hpp>
#include <unwrap.hpp>
#include <cstdlib>
#include <utility>

#include <core/Config.hpp>

using namespace pg;

void pg::panic(const char* func, const char* file, pg::u32 line) {
    std::puts((std::string("[PANIC]: Generated from ") + func + '\n').data());
    auto err = (pg::Error<PanicError>(PanicError{}, "PanicError", func, file, line));
    err.burst();
    pg::panic();
}
