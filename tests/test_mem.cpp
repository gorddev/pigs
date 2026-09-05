#include "filesystem/path.hpp"
#include <print>
#include <pigs.h>

pg::AppStatus init(pg::Engine& e) {

  e.config.dev.no_loop = true;

  std::println("Exec path:\n  {}", e.exec.executable_dir());
  std::println("Current assets folder:\n  {}", pg::files::p_buffer.view());
  std::println("Command-Line Args:");
  for (auto& arg: e.exec.cargs) {
    std::println("  {}", arg);
  }

  std::println(
    "Memory statistics:\n"
    "  size: {}\n"
    "  max_alloc_size: {}\n"
    "  mem_usage: {}\n"
    "  mem_avail: {}",
    e.exec.size().format(), e.exec.max_alloc_size().format(), e.exec.mem_usage().format(), e.exec.mem_avail().format());

  return pg::APP_QUIT;
}
