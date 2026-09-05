#pragma once

#include <string>
#include <ctime>
#include <memory>
#include <source_location>

#include <errors/panic.hpp>

namespace pg {


    #define PG_Err(...) pg::Err(__PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
    #define PG_UErrNew(...) std::unexpected(std::make_unique<pg::Err>(__PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__))
    #define PG_ReturnExpected(err) \
        if (!(err)) { \
            err.error()->add(__PRETTY_FUNCTION__, __FILE__, __LINE__); \
            return std::unexpected(std::move(err.error())); \
        } return (err)
    #define PG_ErrAdd(err, ...) \
        (err).add(__PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
    #define PG_UErrAdd(err, ...) \
        if (!err) \
            (err).error()->add(__PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)

    #define PG_UnsafeReturnUnexpected(err) \
        err.error()->add(__PRETTY_FUNCTION__, __FILE__, __LINE__); \
        return std::unexpected(std::move(err.error()))
    #define PG_ReturnIfUErr(err) \
        if (!err) {\
            err.error()->add(__PRETTY_FUNCTION__, __FILE__, __LINE__); \
            return std::unexpected(std::move(err.error())); \
        }
    #define PG_ReturnIfUErr_m(err, ...) \
        if(!err) { \
            err.error()->add(__PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__); \
            return std::unexpected(std::move(err.error()); \
        }


    /** Error class that handles the propagation of errors through function return calls.
     *
     * Member functions:
     *
     *   - @code add(char[], Args&&...)@endcode Adds an error to the current error chain. You should
     *   call this with @code PG_ReturnIfUErr(err, ...)@endcode, which automatically returns an expected upon failure.
     *
     *   - @code what()@endcode Returns the underlying string buffer of the error.
     *
     *   - @code squash()@endcode Prevents the error from terminating the program if it is not handled.
     *
     * Pseudo-Member Functions:
     *
     *   - @code PG_Err(...)@endcode. Creates a pg::Err object with the first error given by the @code ...@endcode
     *
     *   - @code PG_UErrNew(...)@endcode. Create a new std::unexpected() object with the error inside.
     *
     *   - @code PG_ReturnIfUErr(err, ...)@endcode. Returns the error if the provided pg::expected object has an error inside.
     *
     *   - @code PG_Unwrap(err, ...)@endcode. Attempts to unwrap the expected or optional error.
     *
     * @warning Moving an Err squashes the original error and prevents it from terminating.
     */
    class Err {
    public:
        using ccp = const char*;
        /**
         * @tparam Args Error message to turn into a string.
         * @param loc The location
         * @param args Error message that will be concatenated into the error object. For example, ("hello", 3, "beep") will compile.
         * @return This Error object instance.
         */
        template<typename...Args>
        constexpr Err&& add(ccp func, ccp file, uint_least32_t line, Args&&...args);

        /**
         * @tparam Args Types of arguments to be converted into a string error.
         * @param loc Source location of the original call.
         * @param args Arguments to be converted into the error message.
         */
        template<typename...Args>
        constexpr explicit Err(ccp func, ccp file, uint_least32_t line, Args&&... args);

        /* Default constructable error */
        Err() = default;

        /**
         * @return The underlying error string that can be printed to a console.
         * Retrieving the error prevents the error from terminating the program.
         */
        std::string_view what();
        /** Prints a display string to the console. */
        std::string_view display_str();
        /** Prevents the error from terminating the program if nothing is done about it. */
        Err&& squash();

        /** True if there is an error contained within this object */
        [[nodiscard]] bool is_evil() const;

        /** Terminates the current error immediately if there is an error inside.
         * @param panic_on_empty_err If true, the error will terminate even if there is no error inside.
         */
        Err& burst(bool panic_on_empty_err = false);

        ~Err();
        // 1. Explicit Copy Constructor
        Err(const Err& o);
        // 2. Explicit Copy Assignment Operator
        Err& operator=(const Err& o);

        Err& operator=(Err&& o) noexcept;

        Err(Err&& o) noexcept;

    private:
        std::string err;
        std::string_view err_what = "";
        uint32_t chain_count      = 0u;
        mutable bool suppressed   = true;
        bool error_inside         = false;

        template<typename >
        struct is_valid_string_addition_operator : std::false_type{};
        template<typename T> requires requires(T t) {std::string("") + t;}
        struct is_valid_string_addition_operator<T> : std::true_type{};

        [[noreturn]] void print_and_panic() const {
            std::puts(err.data());
            panic();
        }
    };


    template<typename ... Args>
    constexpr Err&& Err::add(const ccp func, const ccp file, const uint_least32_t line, Args&&... args) {
        suppressed   = false;
        error_inside = true;
        if (chain_count > 100) return std::move(*this);
        if (!chain_count) err += "──────────\x1B[35m\x1B[1m{HEAD}\x1B[0m───────────\n";
        if (chain_count==100) {
            err += "... call chain limit of 100 exceeded ...";
            chain_count++;
            return std::move(*this);
        }
        if (chain_count) {
            uint32_t
                    c = chain_count,
                    d = 1u;
            while (c/10!=0) { c/=10; d--; }
            err += "⟨ " + std::string(d, ' ') + '+' + std::to_string(chain_count) + " ⟩\x1B[90m─┬─\x1B[0m⟨ ";
        } else err += "⟨ src ⟩\x1B[90m─┬─\x1B[0m⟨ \x1B[1m\x1B[33m";
        err = err + func + "\x1B[0m\n        \x1B[90m╰─⟨ ";
        const time_t timestamp = time(nullptr);
        err = err + file + ':' + std::to_string(line) + ' ' + ctime(&timestamp) + "\x1B[31m";

        // then we set up the data for the first error
        const char* err_start = err.data() + err.length();
        size_t err_len = err.length();
        // Add everything to the error
        template for (const auto& p: {std::forward<Args>(args)...}) {
            if constexpr(is_valid_string_addition_operator<std::remove_reference_t<decltype(p)>>::value) {
                err += p;
            } else { err += std::to_string(p); }
        }

        // If there's a real error message we add it.
        if constexpr (sizeof...(args)!=0) {
            err += "\n\x1B[90m───────────────────────────\x1B[0m\n";
        } else err += "\x1B[0m";

        // Set up the err_what message
        err_len = err.length() - err_len;
        if(chain_count == 0)
          this->err_what = std::string_view(err_start, err_len);

        // Add one to the total chain count for the display string.
        chain_count++;
        return std::move(*this);
    }

    template<typename ... Args>
    constexpr Err::Err(ccp func, ccp file, uint_least32_t line, Args&&... args) {
        this->add(func, file, line, std::forward<Args>(args)...);
    }
}
