#pragma once

#include <string_view>
#include "TokenType.hpp"

namespace pg::script {

	struct Token {
	  std::string_view str;
	  unsigned int line;
	  TokenType type;
	};

}
