#pragma once

#include <errors/panic.hpp>
#include <memory>
#include <toolkit/intdef.h>
#include <variant>
#include <vector>
#include <span>
#include <string>
#include <ctime>
#include <expected>
#include <optional>
#include <toolkit/concepts/has_str_function.hpp>

namespace pg {
  using strv = std::string_view;
}

/// Creates a new error at the given location
#define PG_ErrNew(err_t, ...) \
  {err_t{__VA_ARGS__}, #err_t, __PRETTY_FUNCTION__, __FILE__, __LINE__}
/// Creates a new Expected error at the given location
#define PG_UErrNew(err_t, ...) \
  pg::UnexpectedProxy<err_t>{err_t{__VA_ARGS__}, #err_t, __PRETTY_FUNCTION__, __FILE__, __LINE__}
/// Returns a UErr from an optional error if the error exists in the optional.
#define PG_ReturnUErrFromOpt(err) \
	if (err)\
		return std::unexpected(std::move((err).add_trace(__PRETTY_FUNCTION__, __FILE__, __LINE__)))
/// Immediately returns if the given expected value is missing.
#define PG_ReturnIfUErr(expected) \
  if (!expected) {\
    expected.error().add_trace(__PRETTY_FUNCTION__, __FILE__, __LINE__);\
    return std::move(expected).error();\
  }
/// Immediately returns if the optional error value is held.
#define PG_ReturnIfOpt()\
 	if (expected) {\
    (*expected).add_trace(__PRETTY_FUNCTION__, __FILE__, __LINE__);\
    return std::move(*expected);\
  }
#define PG_ReturnUErrUnsafe(expected) \
	expected.error().add_trace(__PRETTY_FUNCTION__, __FILE__, __LINE__); \
	return std::move(expected).error()
/// Crashes the program with the given error.
#define PG_ErrBurst(err_t, ...) \
  pg::Error<err_t>{err_t{__VA_ARGS__}, #err_t, __PRETTY_FUNCTION__, __FILE__, __LINE__}.burst()

namespace pg::err {

  /// Contains information regarding the location in the source code of each function called,
  /// as well as the timestamps of each function call.
  struct StackTrace {
    struct FuncInstance {
      const time_t timestamp;
      const std::string_view func;
      const std::string_view file;
      const u32 line;
    };
  private:
    std::vector<FuncInstance> storage;
    constexpr std::string_view clean_func(std::string_view pretty_func) {
        size_t paren = pretty_func.find('(');
        if (paren == std::string_view::npos) {
            return pretty_func;
        }
        std::string_view prefix = pretty_func.substr(0, paren);
        size_t last_space = prefix.rfind(' ');
        if (last_space == std::string_view::npos) {
            return pretty_func.substr(0, pretty_func.find('('));
        }
        return pretty_func.substr(last_space + 1);
    }
  public:
    /// Creates the stack trace object with a starting function
    StackTrace(std::string_view func, std::string_view file, u32 line) {
      storage.emplace_back(time(nullptr), clean_func(func), file, line);
    }
    /// Adds a trace to the current stack trace
    void add_trace(std::string_view func, std::string_view file, u32 line) {
      storage.emplace_back(time(nullptr), clean_func(func), file, line);
    }
    /// Returns the current number of stack traces
    size_t size() const {
      return storage.size();
    }
    /// Returns a span containing all the function instances.
    std::span<const FuncInstance> get_trace() const {
      return storage;
    }
  };
}

namespace pg {

  template<typename... ErrT>
    requires(pg::concepts::all_have_str_method<ErrT...>)
  class Error {
  private:
    struct Data {
      err::StackTrace trace;
      std::variant<ErrT...> err;
      std::string_view err_type_str;
      bool terminate_on_destruction = true;
    };
    std::unique_ptr<Data> data;

    // FIX 1: The friend declaration must have the identical requires constraint clause!
    template<typename... Ts>
      requires(pg::concepts::all_have_str_method<Ts...>)
    friend class Error;

  public:
    Error(std::variant<ErrT...>&& err, strv err_t_str = "", strv func = "", strv file = "", u32 line = 0) :
      data(std::make_unique<Data>(err::StackTrace{func, file, line}, std::move(err), err_t_str)) {}

    template<typename T>
      requires (!std::is_same_v<std::decay_t<T>, Error> && std::is_constructible_v<std::variant<ErrT...>, T>)
    Error(T&& single_err, strv err_t_str = "", strv func = "", strv file = "", u32 line = 0) :
      data(std::make_unique<Data>(err::StackTrace{func, file, line}, std::variant<ErrT...>(std::forward<T>(single_err)), err_t_str)) {}

    template<typename... OtherErrT>
    Error(Error<OtherErrT...>&& other) noexcept {
      if (other.data) {
        data = std::make_unique<Data>(
          std::move(other.data->trace),
          std::visit([](auto&& v) -> std::variant<ErrT...> { return std::forward<decltype(v)>(v); }, std::move(other.data->err)),
          other.data->err_type_str,
          other.data->terminate_on_destruction
        );
        other.data->terminate_on_destruction = false;
      }
    }

    Error(Error&&) noexcept = default;
    Error& operator=(Error&&) noexcept = default;
    Error(const Error&) = delete;
    Error& operator=(const Error&) = delete;

    Error& add_trace(strv func, strv file, u32 line) {
      if (data) {
        data->trace.add_trace(func, file, line);
      }
      return *this;
    }

    template<class... Ts> struct visitor : Ts... { using Ts::operator()...; };
    template<class... Ts> void visit(visitor<Ts...>&& v) {
      std::visit(v, data->err);
    }

    template<typename T>
    T* get_if() {
      return std::get_if<T>(&data->err);
    }

    template<typename T>
    bool holds() {
      return std::holds_alternative<T>(data->err);
    }

    const std::vector<err::StackTrace::FuncInstance>& trace() const {
      return data->trace.raw_storage();
    }

    const err::StackTrace::FuncInstance& source() const {
      return data->trace.get_trace()[0];
    }

    std::string what() {
      data->terminate_on_destruction = false;
      return std::visit([](const auto& val) {
        return val.str();
      }, data->err);
    }

    void squash() {
      data->terminate_on_destruction = false;
    }

    std::string display_str() {
      constexpr auto size_per_trace_estimate = 40;
      std::string what_str = this->what();

      std::string ret;
      ret.reserve(data->trace.size() * size_per_trace_estimate + what_str.length());

      auto trace_span = data->trace.get_trace();
      auto timestamp_0 = trace_span[0].timestamp;

      ret = ret +
        "────\x1B[35m\x1B[1m{HEAD: " + data->err_type_str + "}\x1B[0m────\n"
        "⟨ src ⟩\x1B[90m─┬─\x1B[0m⟨ \x1B[1m\x1B[33m" + trace_span[0].func + "\x1B[0m\n"
        "        \x1B[90m╰─⟨ "
          + trace_span[0].file + ':' + std::to_string(trace_span[0].line)
          + ' ' + ctime(&timestamp_0) +
          "\x1B[31m" + what_str + "\x1B[0m\n";

      for(size_t i = 1; i < data->trace.size(); i++) {
        uint32_t c = i, d = 1u;
        while (c/10!=0) { c/=10; d--; }
        auto timestamp_i = trace_span[i].timestamp;
        ret += "⟨ " + std::string(d, ' ') + '+' + std::to_string(i) + " ⟩\x1B[90m─┬─\x1B[0m⟨ ";
        ret += trace_span[i].func;
        ret +=  "\x1B[0m\n"
                "        \x1B[90m╰─⟨ ";
        ret += trace_span[i].file;
        ret += ':';
        ret += std::to_string(trace_span[i].line) + ' ' + ctime(&timestamp_i) + "\x1B[0m\n";
      }

      return ret;
    }

    std::string_view err_type() {
      return data->err_type_str;
    }

    ~Error() {
      if (this->data && this->data->terminate_on_destruction) {
        std::puts("──────────\x1B[36m\x1B[1m{Warning}\x1B[0m───────────\n"
                  "\x1B[31m\x1B[1merror:\x1B[0m Unhandled "
                  "\x1B[34m[pg::Err]\x1B[0m. Please call either:\n"
                  "\terr.\x1B[3m\x1B[36mwhat\x1B[0m() \x1B[90m(for string "
                  "output)\x1B[0m, or\n"
                  "\terr.\x1B[3m\x1B[36msquash\x1B[0m() \x1B[90m(for error "
                  "suppression)\x1B[0m\n"
                  "To prevent the error from terminating the program.");
        this->burst();
      }
    }

    /// Prints the error to the console and calls exit on the program.
    Error& burst() {
      std::puts(this->display_str().c_str());
      pg::panic();
    }

    template<typename U, typename... TargetErrT>
      requires(pg::concepts::all_have_str_method<TargetErrT...>)
    operator std::expected<U, pg::Error<TargetErrT...>>() && {
      return std::unexpected(
        pg::Error<TargetErrT...>{std::move(*this)}
      );
    }
  };

  template<typename ErrT>
  Error(ErrT&&, strv, strv, strv, u32) -> Error<ErrT>;

  template<typename T, typename... ErrT>
  using expected = std::expected<T, Error<ErrT...>>;

  template<typename T>
  using optional_err = std::optional<pg::Error<T>>;

  template<typename ConcreteErr>
    struct UnexpectedProxy {
      ConcreteErr err_obj;
      std::string_view err_t_str;
      std::string_view func;
      std::string_view file;
      u32 line;

      template<typename T, typename... TargetErrT>
        requires(pg::concepts::all_have_str_method<TargetErrT...>)
      operator std::expected<T, pg::Error<TargetErrT...>>() && {
        return std::unexpected(
          pg::Error<TargetErrT...>(std::move(err_obj), err_t_str, func, file, line)
        );
      }
    };
}
