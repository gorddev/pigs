#pragma once

#include "errors/Error.hpp"
#include "errors/error-structs/scripting/TokenErrors.hpp"
#include "filesystem/dumpFile.hpp"
#include <cctype> // Added for std::isdigit, std::isalpha
#include <cstring>
#include <unordered_map>
#include <vector>

#include <errors/error-structs/files/FileNotExistsError.hpp>
#include <errors/error-structs/files/FileNotOpened.hpp>

namespace pg::script {

enum class TokenType : char {
  // KEYWORDS
  USE = 'u',
  IF = 'C',
  ELIF = 'c',
  ELSE = 'E',
  FOR = 'f',
  WHILE = 'W',
  RETURN = 'R',
  // OPERATIONS
  ASSIGN = '=',
  ADD = '+',
  SUB = '-',
  MUL = '*',
  DIV = '/',
  // LANGUAGE
  IDENTIFIER = 'v',
  DOT = '.',
  NEWLINE = 'l',
  // LITERALS
  STRING = 's', // Added token type for string literals
  NUMBER = 'n'  // Added token type for numeric literals
};

const std::unordered_map<std::string_view, TokenType> KeywordRegistry = {
    {"use", TokenType::USE},      {"if", TokenType::IF},
    {"elif", TokenType::ELIF},    {"else", TokenType::ELSE},
    {"for", TokenType::FOR},      {"while", TokenType::WHILE},
    {"return", TokenType::RETURN}};

struct Token {
  std::string str;
  u32 line;
  TokenType type;
};

static inline bool isWhitespace(char c) {
  switch (c) {
  case ' ':
  case '\t':
  case '\n':
  case '\r':
    return true;
  default:
    return false;
  }
}

/// Returns true if the given character is a special reserved character
static inline bool isSpecialChar(char c) {
  switch (c) {
  case '+':
  case '-':
  case '*':
  case '/':
  case '@':
  case '=':
  case '.':
    return true;
  default:
    return false;
  }
}

static inline Token parseIdentifier(const std::string &str, size_t &index,
                                    u32 line_number) {
  size_t start = index;
  while (index < str.length() && !isWhitespace(str[index]) &&
         !isSpecialChar(str[index])) {
    index++;
  }
  std::string_view view{std::string_view(str.data() + start, index - start)};

  index--;
  if (KeywordRegistry.contains(view))
    return {std::string(view), line_number, KeywordRegistry.at(view)};
  return {std::string(view), line_number, TokenType::IDENTIFIER};
}

static inline Token parseStringLiteral(const std::string &str, size_t &index,
                                       u32 line_number) {
  size_t start = index;
  index++;

  while (index < str.length() && str[index] != '"') {
    if (str[index] == '\\' && index + 1 < str.length()) {
      index += 2;
    } else {
      index++;
    }
  }

  if (index >= str.length()) {
    return {str.substr(start), line_number, TokenType::STRING};
  }

  std::string token_val = str.substr(start, index + 1 - start);
  return {token_val, line_number, TokenType::STRING};
}

static inline expected<Token, script::err::InvalidLiteral> parseNumericLiteral(const std::string &str, size_t &index,
                                        u32 line_number) {
  size_t start = index;
  bool has_decimal = false;

  while (index < str.length()) {
    char c = str[index];
    if (std::isdigit(static_cast<unsigned char>(c))) {
      index++;
    } else if (c == '.') {
      if (has_decimal) {
        return PG_UErrNew(err::InvalidLiteral,
        	.number_str  = std::string(std::string_view(str.data()+start, index - start)),
         	.line_number = line_number,
        	.err_type = err::InvalidLiteral::EXTRA_DECIMAL
        );
      }
      has_decimal = true;
      index++;
    } else {
      break;
    }
  }

  std::string token_val = str.substr(start, index - start);
  index--;
  return Token{token_val, line_number, TokenType::NUMBER};
}




expected<
	std::vector<Token>,
	err::InvalidLiteral,
	pg::err::FileNotExists,
	pg::err::FileNotOpened>
parse(std::string file) {
  std::vector<Token> tokens;
  u32 line_number = 0;

  for (size_t i = 0; i < file.length(); i++) {
    switch (file[i]) {
    // Spaces/breaks
    case '\n':
      line_number++;
      tokens.emplace_back("\\n", line_number, TokenType::NEWLINE);
    case ' ':
    case '\t':
    case '\r':
      break;
    case '.':
      if (i + 1 < file.length() &&
          std::isdigit(static_cast<unsigned char>(file[i + 1]))) {
        auto literal = parseNumericLiteral(file, i, line_number);
        PG_ReturnIfUErr(literal);
        tokens.push_back(*literal);
      } else {
        tokens.emplace_back(".", line_number, TokenType::DOT);
      }
      break;
    case '=':
      tokens.emplace_back("=", line_number, TokenType::ASSIGN);
      break;
    case '+':
      tokens.emplace_back("+", line_number, TokenType::ADD);
      break;
    case '-':
      tokens.emplace_back("-", line_number, TokenType::SUB);
      break;
    case '*':
      tokens.emplace_back("*", line_number, TokenType::MUL);
      break;
    case '/':
      // Fixed buggy TokenType mapping assignment from DIV to *
      tokens.emplace_back("/", line_number, TokenType::DIV);
      break;
    case '"':
      tokens.push_back(parseStringLiteral(file, i, line_number));
      break;

    default:
      if (std::isdigit(static_cast<unsigned char>(file[i]))) {
      	auto literal = parseNumericLiteral(file, i, line_number);
       	PG_ReturnIfUErr(literal);
        tokens.push_back(*literal);
      } else {
        tokens.push_back(parseIdentifier(file, i, line_number));
      }
      break;
    }
  }
  return tokens;
}

expected<
	std::vector<Token>,
	err::InvalidLiteral,
	pg::err::FileNotExists,
	pg::err::FileNotOpened>
parse(const path& p) {
	auto fileExpected = dumpFile(p);
	PG_ReturnIfUErr(fileExpected);
	return parse(*fileExpected);
}

} // namespace pg::script
