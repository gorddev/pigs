#pragma once
#include <vector>
#include <span>


#include "toolkit/platform/memory/memory_usage.hpp"
#include "toolkit/types/void_ptr.hpp"

/* Created by Gordie Novak on 8/23/26.
 * Purpose:
 * Allows the user to easily access properties of executable and state of the runtime */

namespace pg {

    class Exec {
        friend class Engine;
        Exec();
        void init(int argc, char* argv[]);
        std::vector<std::string_view> cli_heap_storage;
    public:
        std::string_view name;              ///< Current name of the executable
        std::span<std::string_view> cargs;  ///< Command-line arguments provided to the engine
        void_ptr udata;                     ///< User-provided data that the engine will keep track of.

        [[nodiscard]] mem::bytes size();            ///< Returns the size of the current executable (in bytes)
        [[nodiscard]] mem::bytes max_alloc_size();  ///< Returns the maximum size of a single allocation (in bytes)
        [[nodiscard]] mem::bytes mem_usage();       ///< Returns the current memory usage of the program. (in bytes). @warning May not be accurate in MaxOS
        [[nodiscard]] mem::bytes mem_avail();       ///< Returns the current system memory available to the program (in bytes)
        [[nodiscard]] std::string executable_dir(); ///< Returns the directory containing the current executable
    };

}
