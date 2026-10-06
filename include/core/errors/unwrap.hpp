#pragma once

#include "Error.hpp"
#include <expected>
#include <optional>

namespace pg {
#define PG_Err_Param_t const char *func, const char *file, uint_least32_t line

/// Crashes the program at the given location with the given error messages.
#define PG_Panic(...)                                                          \
  pg::panic(__PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define PG_Unwrap(err) pg::unwrap(err, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define PG_CWarn_(warning, cond, ...)                                          \
  if (pg::opt_nmspc::pg_internal_options.warn.warning && (cond))               \
  pg::warn(#warning, __VA_ARGS__)
#define PG_Warn_(warning, ...)                                                 \
  if (pg::opt_nmspc::pg_internal_options.warn.warning)                         \
  pg::warn(#warning, __VA_ARGS__)

template <typename T, typename... ErrT>
T &&unwrap(pg::expected<T, ErrT...> &&err, PG_Err_Param_t) {
  if (!err) {
    std::puts((std::string("[PANIC] called from ") + func).data());
    err.error().add_trace(func, file, line).burst();
  }
  return std::move(*err);
}

template <typename T, typename... ErrT>
T &unwrap(expected<T, ErrT...> &err, PG_Err_Param_t) {
  if (!err) {
    std::puts((std::string("[PANIC] called from ") + func).data());
    err.error().add_trace(func, file, line).burst();
  }
  return *err;
}

struct UnwrapError {
  std::string str() const { return "Unwrapped the std::nullopt."; }
};

template <typename T> T &&unwrap(std::optional<T> &&t, PG_Err_Param_t) {
  if (t == std::nullopt) {
    std::puts("[PANIC] called from pg::unwrap.");
    auto err =
        (Error<UnwrapError>(UnwrapError{}, "UnwrapError", func, file, line));
    err.burst();
  } else
    return std::move(*t);
}

template <typename T> T &unwrap(std::optional<T> &t, PG_Err_Param_t) {
  if (t == std::nullopt) {
    std::puts("[PANIC] called from pg::unwrap.");
    auto err =
        (Error<UnwrapError>(UnwrapError{}, "UnwrapError", func, file, line));
    err.burst();
  } else
    return *t;
}

struct PanicError {
  std::string val;
  std::string str() const { return val; }
};

/// Throws an error from a specific source.
void panic(PG_Err_Param_t);

template <typename>
struct is_valid_string_addition_operator : std::false_type {};
template <typename T>
  requires requires(T t) { std::string("") + t; }
struct is_valid_string_addition_operator<T> : std::true_type {};

template <typename... MSGS>
[[noreturn]] void panic(PG_Err_Param_t, [[maybe_unused]] MSGS &&...msgs) {
  std::string msg;
  (([&msg](const auto& p) mutable {
	 	if constexpr (is_valid_string_addition_operator<std::remove_reference_t<decltype(p)>>::value) {
	    msg += p;
	  } else {
	    msg += std::to_string(p);
	  }
  })(std::forward<MSGS>(msgs)), ...);
  std::puts((std::string("[PANIC]: Generated from") + func + '\n').data());
  pg::Error<PanicError>(PanicError{.val = msg}, "PanicError", func, file, line)
      .burst();
  pg::panic();
}

template <typename... MSGS>
void warn(const std::string_view warning_type, MSGS &&...msgs) {
  std::string warn = "⟨ \x1B[35m\x1B[1mpgWARN\x1B[0m\x1B[0m ⟩─┬─⟨ ";
  warn.reserve(65 + sizeof...(MSGS) * 5);
  (([&warn](const auto& p) mutable {
	 	if constexpr (is_valid_string_addition_operator<std::remove_reference_t<decltype(p)>>::value) {
	    warn += p;
	  } else {
	    warn += std::to_string(p);
	  }
  })(std::forward<MSGS>(msgs)), ...);
  warn += "\x1B[0m\n           ╰─⟨ \x1B[35m\x1B[3m[config.warn.\x1B[01m";
  warn += warning_type;
  warn += "\033[24m]\x1B[0m";
  std::puts(warn.c_str());
}

#undef PG_Err_Param_t
} // namespace pg
