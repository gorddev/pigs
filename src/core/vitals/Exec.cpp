#include "errors/unwrap.hpp"
#include <core/vitals/Exec.hpp>
#include <cstring>

#include "toolkit/platform/executable_dir.hpp"
#include "toolkit/platform/memory/format_memory.hpp"
#include "toolkit/platform/memory/memory_usage.hpp"

using namespace pg;

Exec::Exec() : cargs(cli_heap_storage), udata{} {}

void Exec::init(int argc, char *argv[]) {
  if (argc <= 0)
    PG_Panic("There's no command-line arguments. argc = ", argc);

  cli_heap_storage.reserve(argc);
  for (int i = 0; i < argc; i++) {
    cli_heap_storage.emplace_back(argv[i], strlen(argv[i]));
  }
  name = cli_heap_storage[0];
  cargs = cli_heap_storage;
}

mem::bytes Exec::size() { return pg::mem::get_executable_size(); }

mem::bytes Exec::max_alloc_size() { return mem::get_max_allocation_size(); }

mem::bytes Exec::mem_usage() { return mem::get_process_memory_usage(); }

mem::bytes Exec::mem_avail() { return mem::get_system_memory_available(); }

std::string Exec::executable_dir() { return pg::platform::getExecutableDir(); }
