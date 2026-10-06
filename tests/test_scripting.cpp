#include <cstdint>
#include <pigs.h>
#include <pigs_init.h>

#include <core/scripting/tokens/Tokenizer.hpp>
#include "toolkit/containers/SettingVar.hpp"

#include <toolkit/intdef.h>
#include <core/scripting/AST/Parser.hpp>

#include <chrono>
#include "toolkit/serialization/enumToString.hpp"
#include <toolkit/serialization/serialize.hpp>
#include <iostream>


template<typename Func, typename...Args>
uintmax_t time(Func&& f, Args&&... args) {
	auto start = std::chrono::steady_clock::now();
	f(std::forward<Args>(args)...);
	auto now = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
}

template<size_t N, typename Func, typename...Args>
	requires (N != 0)
uintmax_t time_test(Func&& f, Args&&... args) {
	uintmax_t count = 0;
	for (size_t i = 0; i < N; i++) {
		count += time(f, std::forward<Args>(args)...);
	}
	return count;
}


pg::AppStatus init(pg::Engine& e) {
	e.config.assets_folder = "../../tests/assets";
  // We will not have a main loop.
  std::cout << "test1: " << time_test<100>([&e] {
  	e.config.assets_folder = "../../tests/assets";
  }) << std::endl;

  std::cout << "test2: " << time_test<100>([&] {
  	e.config.assets_folder = "../../tests/assets";
  }) << std::endl;

  std::cout << "test3: " << time_test<100>([&e] {
  	e.config.assets_folder = "../../tests/assets";
  }) << std::endl;

  std::cout << "test4: " << time_test<100>([&] {
  	e.config.assets_folder = "../../tests/assets";
  }) << std::endl;


  pg::script::Tokenizer tokenizer;
  pg::script::ASTPtr head;
  auto token_time = time([&tokenizer, &head]() mutable {
	  tokenizer = PG_Unwrap(pg::script::Tokenizer::make("script_test.gat"));
	  head = pg::script::Parser(tokenizer.get_tokens()).parse();
	  auto end = std::chrono::system_clock::now();
  });
 //pg::script::ASTPrinter::print(*head);

  std::cout << "time_to = " << token_time << std::endl;


  return pg::APP_QUIT;
}
