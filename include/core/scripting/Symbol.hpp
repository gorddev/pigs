#pragma once

#include "errors/Error.hpp"
#include "errors/unwrap.hpp"
#include "toolkit/concepts/assignment_type.hpp"
#include "toolkit/serialization/enumToString.hpp"
#include "toolkit/serialization/serialdef.hpp"
#include "toolkit/intdef.h"
#include "toolkit/types/void_ptr.hpp"
#include <any>
#include <functional>
#include <string>
#include <vector>
#include <iostream>

#include "script_conversion.hpp"

namespace pg::script {

  struct Symbol {
  public:
    std::string name;
    ser::bin_t type;

  private:
    void* data_ptr = nullptr;
    // Type-erased setter that retains the original wrapper instance type context
    void (*setter_thunk)(void* target_ref, const void* src_val, ser::bin_t src_type) = nullptr;

  public:
    Symbol() = default;

    template<typename T>
    Symbol(std::string_view name, T& t) :
      name(name),
      type(ser::typeToEnum<concepts::assignment_type<T>>()),
      data_ptr(static_cast<void*>(std::addressof(t)))
    {
      // Capture T (the true property wrapper type) into the thunk
      setter_thunk = [](void* target_ref, const void* src_val, ser::bin_t src_type) {
          // Cast target back to the exact class object (preserving onModify hooks)
          auto& original_wrapper = *static_cast<T*>(target_ref);

          // Find out what type src_val actually is using TypeSwitch
          PG_TypeSwitch_(src_type,
              if constexpr (std::is_convertible_v<Tm, concepts::assignment_type<T>>) {
                  // This calls original_wrapper.operator=(...), triggering the hook!
                  original_wrapper = *static_cast<const Tm*>(src_val);
              } else {
                  PG_Panic("Incompatible type in thunk assignment.");
              }
          , PG_Panic("Invalid type index in thunk."););
      };

    }

    template<typename T>
    [[nodiscard]] pg::Err set(const T& t) {
      auto err = script::isConvertibleTo(t, this->type);
      if (err.is_evil())
        return PG_ErrAdd(err, "Conversion error for symbol \"", this->name, "\"");

      // Pass the address of the incoming value, and its type tag, to the thunk
      this->setter_thunk(this->data_ptr, &t, ser::typeToEnum<T>());
      return err;
    }

    template<typename T>
    [[nodiscard]] pg::Err get(T& out_val) const {
      auto err = pg::Err{};

      PG_TypeSwitch_(this->type,
        if constexpr (std::is_convertible_v<Tm, T>) {
          // If T represents a wrapper class, casting it to its underlying target primitive works here
          const auto& underlying_var = *static_cast<const Tm*>(this->data_ptr);
          out_val = static_cast<T>(underlying_var);
        } else {
          PG_Panic("Incompatible type reading attempted inside TypeSwitch.");
        }
        , PG_Panic("Oops not supposed to be here.");)

      return err;
    }

    template<typename T>
    Symbol& operator=(const T& t) {
      (void)this->set(t);
      return *this;
    }
  };

}
