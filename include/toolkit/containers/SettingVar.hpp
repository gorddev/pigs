// Establishes a settings variable that can dynamically update
// when modified.
#pragma once
#include <functional>
#include <type_traits>
#include <string>
#include <vector>

#include <toolkit/serialization/concepts/is_vector.hpp>

#include "toolkit/concepts/has_+-*div_operator.hpp"

namespace pg {

  // Returned during a callback: whether the variable should be set or not.
  enum class SettingOpt : bool {
    SET = true,
    DONT_SET = false
  };

  template<typename T>
  concept is_valid_setting_type =
    (std::is_trivially_copyable_v<T> || ser::is_vector_v<T>) &&
    (sizeof(T) < std::numeric_limits<short>::max());

  // The actually setting class
  template<typename T>
  class SettingVar {
  public:
    using callbackFunc = std::function<SettingOpt(const T&)>;
    using value_type = T;

    SettingVar() = default;
    SettingVar(const T& default_value)
      : setting(default_value), callbacks() {}

    template<size_t N>
    requires(std::is_same_v<T, std::string>)
    SettingVar(const char (&arr)[N]) : setting(arr) {}


    /// Sets the underlying setting value while calling all callbacks.
    SettingVar& set(const T& var) {
      bool update_var = true; // Whether or not we end up updating the variable
      for (auto& func: callbacks) {
        update_var = update_var && static_cast<bool>(func(var));
      }
      if (update_var) setting = var;
      return *this;
    }

    SettingVar& operator=(const T& var) {
      set(var);
      return*this;
    }

    template<size_t N>
    requires(std::is_same_v<T, std::string>)
    SettingVar& set(const char (&var)[N]) {
      bool update_var = true; // Whether or not we end up updating the variable
      for (auto& func: callbacks) {
        update_var &= static_cast<bool>(func(var));
      }
      if (update_var) setting = var;
      return *this;
    }

    template<size_t N>
    requires(std::is_same_v<T, std::string>)
    SettingVar& operator=(const char (&var)[N]) {
      set(var);
      return *this;
    }

    /// Adds a callback upon variable being set
    SettingVar& onModify(callbackFunc&& func) {
        callbacks.push_back(std::move(func));
        return *this;
    }

    // Overload for parameterless functions returning void (automatically updates the variable)
    template<typename F>
    requires (std::is_invocable_r_v<void, F> && !std::is_invocable_r_v<SettingOpt, F>)
    SettingVar& onModify(F func) {
      callbacks.push_back([func = std::move(func)](const T&) {
        func();
        return SettingOpt::SET;
      });
      return *this;
    }

    // Overload for parameterless functions returning a SettingOpt
    template<typename F>
    requires (std::is_invocable_r_v<SettingOpt, F> && !std::is_invocable_v<F, const T&>)
    SettingVar& onModify(F func) {
      callbacks.push_back([func = std::move(func)](const T&) {
        return func();
      });
      return *this;
    }

    // Returns the size in bytes of an object
    size_t size_in_bytes() {
      if constexpr (std::is_same_v<T, std::string>) {
        return this->setting.length();
      } else {
        return sizeof(T);
      }
    };

    // Returns a constant reference to the underlying setting value
    [[nodiscard]] const T& value() {
      return this->setting;
    }

    /// Automatically converts to the underlying value.
    operator const T&() { return setting; } // NOLINT(*-explicit-constructor)

    /// Returns the underlying value;
    const T& operator()() {return setting;}

    /// Allows for equality operations
    bool operator==(const T& other) requires std::equality_comparable<T> { return setting == other;}
    bool operator!=(const T& other) requires std::equality_comparable<T> { return setting != other;}

    SettingVar& operator+=(const T& other) requires concepts::has_add_op<T>  { return set(setting + other); }
    SettingVar& operator-=(const T& other) requires concepts::has_sub_op<T>  { return set(setting - other); }
    SettingVar& operator*=(const T& other) requires concepts::has_mult_op<T> { return set(setting * other); }
    SettingVar& operator/=(const T& other) requires concepts::has_div_op<T>  { return set(setting / other); }

  public:
    friend class Config;

    auto* data() {
      if constexpr (ser::is_vector_v<T>) {
        return std::data(setting);
      } else {
        return &this->setting;
      }
    }

    T setting;                            // The actual setting variable
    std::vector<callbackFunc> callbacks;  // Vector containing all the callback functions.
  };
}
