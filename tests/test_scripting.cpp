#include "callbacks/AppStatus.hpp"
#include "scripting/tokenizer.hpp"
#include "toolkit/containers/SettingVar.hpp"
#include <pigs.h>
#include <pigs_init.h>

constexpr const char script[] = R"(
use Engine
x = 5
str = "stringo: \"string\""
)";

pg::AppStatus init(pg::Engine& e) {
  // We will not have a main loop.
  e.config.dev.no_loop = true;

  pg::SettingVar<int> x = 12;

  e.config.vsync.onModify([]() {
    std::cerr << "here we go!" << std::endl;
  });

  auto parsed = pg::script::parse(std::string(script));
  if (!parsed) parsed.error().burst();
  for (const auto& token: *parsed) {
  	std::cerr <<
   		'{' << (char)token.type << ':' << token.str << ':' << token.line << '}';
   	if (token.type==pg::script::TokenType::NEWLINE)
    	std::cerr << std::endl;
  }

  std::cerr << "parsed\n";

  return pg::APP_QUIT;
}
