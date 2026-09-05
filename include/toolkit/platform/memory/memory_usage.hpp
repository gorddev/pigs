#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <toolkit/platform/memory/format_memory.hpp>

namespace pg::mem {

struct bytes {
  std::uintmax_t n_bytes;

  std::string format(Unit unit = AUTO, size_t precision = 4, bool space = false) {
    return mem::formatBytes(n_bytes, unit, precision).to_string(space);
  }

  template<typename T>
    requires(std::is_unsigned_v<T>)
  operator T() { return n_bytes; }
};

/** Returns the maximum executable size of this specific file.
 * @return The current executable's size in memory. 0 if file is not found or
 * running on the web.*/
bytes get_executable_size();

/** @return The maximum allocation size of a single allocation that can
 * currently take place. */
bytes get_max_allocation_size();

/** @return The current process' memory usage. Sketchy for MacOS */
bytes get_process_memory_usage();

/** @return The total amount of system memory available */
bytes get_system_memory_available();

} // namespace pg::mem
