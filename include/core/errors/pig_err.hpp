#pragma once

#include <expected>

#include "pig_log.hpp"
#include <iostream>
#include <optional>

#include "Error.hpp"

/// The err namespace
namespace pg {
    /// Throws an error from a specific source.
    template<typename... MSGS>
    void panic(const char src[], MSGS&&... msgs) {
        std::cerr << ("\n--------------\n!!PANIC!! Generated from: [")
            << src << "]\n--------------\n";
        if constexpr (sizeof...(MSGS) > 0)
            PIG_AppendLog(src, std::forward<MSGS>(msgs)...);
        std::cerr << PIG_GetLog() << std::endl;
        throw std::runtime_error("");
    }

    /** Throws a panic if the Errcode enum is @code ERR@endcode or @code PANIC@endcode
     * @param e The errcode you want to test
     * @param src The source of the error
     * @param reason The reason for calling the panic (if unsuccessful) */
    template<typename... MSGS>
    void panic_if(Errcode e, const char src[], MSGS&&... reason) {
        if (e) return;
        if constexpr (sizeof...(MSGS) > 0)
            PIG_AppendLog(src, std::forward<MSGS>(reason)...);
        std::cerr << PIG_GetLog() << std::endl;
        throw std::runtime_error("");
    }
    /// Unwraps an optional. If unsuccessful, throws an exception.
    /// @param src The source of the error
    /// @param reason The reason for calling unwrap.
    template<typename T, typename... Args>
    T unwrap(std::optional<T>&& opt, const char* src = "?", Args&&... reason) {
        if (!opt) {
            if constexpr (sizeof...(Args) == 0)
                PIG_AppendLog(src, "Failed unwrap of optional<T>.");
            else
                PIG_AppendLog(src, reason...);
            panic(src);
        }
        return std::move(opt.value());
    }

    template<typename T, typename... Args>
    T unwrap(std::expected<T, Error> exp, const char* src = "?", Args&&... reason) {
        if (!exp) {
            if constexpr (sizeof...(Args) == 0)
                PIG_AppendLog(src, "Failed unwrap of expected<T, Error>.");
            else
                PIG_AppendLog(src, reason...);
            panic(src);
        }
        return std::move(exp.value());
    }

    #define PG_UNWRAP(val) \
        pg::unwrap(val, CURRENT_SCOPE)
}

