#pragma once
#include <string_view>

#include "pig_log.hpp"


#if defined(__GNUC__) || defined(__LOG4CPP__)
    #define CURRENT_SCOPE __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define CURRENT_SCOPE __FUNCSIG__
#else
    #define CURRENT_SCOPE __func__ // Falls back to function name only
#endif


namespace pg {

    struct Error {
        std::string_view what() const {
            return PIG_GetLogString();
        }

        // Creates an error object with a given error
        #define PG_MAKE_ERR(...) \
            (PIG_WriteLog(CURRENT_SCOPE, __VA_ARGS__), Error{})

        // Creates an unexpected error with the given parameters
        #define PG_MAKE_UERR(...) \
            (PIG_WriteLog(CURRENT_SCOPE, __VA_ARGS__), std::unexpected(Error{}))

        // Continues the propagation of a specific error through the error chain.
        #define PG_CONTINUE_ERR(Err, ...) \
            (PIG_AppendLog(CURRENT_SCOPE, __VA_ARGS__), Err)

        // Continues the propagation of a specific error through the error chain.
        #define PG_CONTINUE_UERR(Err, ...) \
            (PIG_AppendLog(CURRENT_SCOPE, __VA_ARGS__), std::unexpected(Err.error()))

    };


}
