#pragma once

#include <toolkit/types/fstring.hpp>
#include <type_traits>
#include "debug.hpp";

// made by gordie feb 20th

namespace pg {
    /// Enum representing whether something was ok, or something errored
    enum Errcode {
        ERR = 0, PANIC = 0,
        OK = 1, SUCCESS = 1
    };
}

/// Holds the error buffer and number of functions called before the program is meant to error.
/// Contains an internal "call_chain" num (number of functions called) and a character buffer of size 1024.
class PIG_internal_log_handler {
public:
    constexpr static uint16_t buff_size = 1024;
private:
    inline static pg::fstring<buff_size> error_buffer;   ///< "Fixed String" of size 1024 that contains the errors.
    inline static uint16_t call_chain;          ///< Number of functions called.

    // friends
    friend inline const char* PIG_GetLog();
    friend inline void PIG_ClearLog();
    friend inline pg::fstring<buff_size>& PIG_RawAppendLog();
    // friends

    static const char* getError_c_str() {
        return error_buffer.c_str();
    }

    static pg::fstring<buff_size>& get_raw_buffer() {
        return error_buffer;
    }

public:

    // deleted constructors
    PIG_internal_log_handler(const PIG_internal_log_handler&) = delete;             //
    PIG_internal_log_handler& operator=(const PIG_internal_log_handler&) = delete;  //
    PIG_internal_log_handler(const PIG_internal_log_handler&&) = delete;            //
    PIG_internal_log_handler& operator=(const PIG_internal_log_handler&&) = delete; //
    // deleted constructors

    static void write_log(const char err[]) {
        error_buffer = err;
    }

    static uint16_t get_call_chain_count() {
        return call_chain;
    }

    static void increment_call_chain_count() {
        call_chain++;
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    static bool has_error() {
        return !error_buffer.empty();
    }

    static uint16_t get_length() {
        return error_buffer.length();
    }
};

/// Clears the current error in the error buffer & resets the call chain.
inline void PIG_ClearLog() {
    PIG_internal_log_handler::error_buffer.clear();
    PIG_internal_log_handler::call_chain = 0;
}

/// Gets the raw underlying fstring of the error buffer.
inline pg::fstring<PIG_internal_log_handler::buff_size>& PIG_RawAppendLog() {
    return PIG_internal_log_handler::get_raw_buffer();
}

/// Appends an error to the current buffer, increasing the call chain count by one.
template<typename... Args>
void PIG_AppendLog(const char src[], Args&&... args) {
    auto& buffer = PIG_RawAppendLog();
    auto chain = PIG_internal_log_handler::get_call_chain_count();
    if (chain) {
        buffer << "\n------------------------------\n";
        buffer << "> +" << chain << "   [" << src << "]\n";
    }
    else {
        buffer << "> src" << " [" << src << "]\n";
    }
    (..., (buffer << args));
    PIG_internal_log_handler::increment_call_chain_count();
}

template<typename T>
concept PIG_can_error =
    std::is_convertible_v<T, const char*>
    || std::is_fundamental_v<T>
    || std::is_convertible_v<T, unsigned>
    || std::is_unsigned_v<T>
    || std::is_convertible_v<T, const std::string_view&>;
/// Writes an error to the error buffer, overwriting the current error & resetting the call chain count.
template<typename... Args>
    requires (PIG_can_error<Args> && ...)
void PIG_WriteLog(const char src[], Args&&... args) {
    PIG_ClearLog();

    PIG_internal_log_handler::write_log("> src [");

    PIG_RawAppendLog() << src << "]\n";
    (..., (PIG_RawAppendLog() << args));

    PIG_internal_log_handler::increment_call_chain_count();
}

/// Gets the current error stored in the error buffer. If no error is stored, checks OpenGL for an error.
/// Otherwise returns blank.
inline const char* PIG_GetLog() {
    if (PIG_internal_log_handler::has_error()) {
        return PIG_internal_log_handler::getError_c_str();
    } return "";
}

inline std::string_view PIG_GetLogString() {
    if (PIG_internal_log_handler::has_error()) {
        return {PIG_GetLog(), PIG_internal_log_handler::get_length()};
    } return "";
}