#pragma once

#include <cctype>
#include <cstring>
#include <vector>
#include <core/filesystem/path.hpp>

#include <core/errors/Error.hpp>
#include <core/errors/err-types/files/FileErrors.hpp>
#include <core/errors/err-types/scripting/TokenErrors.hpp>

#include "Token.hpp"

namespace pg::script {


class Tokenizer {
	std::string script;
	std::vector<Token> tokens;
	mutable size_t token_index = 0;

public:
	Tokenizer() = default;

	static expected<
		Tokenizer,
		script::err::InvalidLiteral,
		script::err::UnterminatedString,
		pg::err::FileNotExists,
		pg::err::FileNotOpened>
	make(const pg::path& p);

	const Token& peek() const {
		return tokens[token_index];
	}

	const Token& advance() const {
		return tokens[token_index++];
	}

	bool eof() const {
		return token_index == tokens.size();
	}

	std::span<const Token> get_tokens(this Tokenizer& self) {
		return self.tokens;
	}

	std::string_view get_script(this Tokenizer& self) {
		return self.script;
	}

private:
	Tokenizer(std::string&& str, std::vector<Token>&& tokens);

};
}
