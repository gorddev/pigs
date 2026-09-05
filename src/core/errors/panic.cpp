#include <errors/panic.hpp>
#include <errors/unwrap.hpp>
#include <cstdlib>


using namespace pg;

[[noreturn]] void pg::panic() {
    std::exit(1);
}

void pg::panic(const char* func, const char* file, pg::u32 line) {
    std::puts((std::string("[PANIC]: Generated from") + func + '\n').data());
    auto err = (pg::Error<PanicError>(PanicError{}, "PanicError", func, file, line));
    err.burst();
}
