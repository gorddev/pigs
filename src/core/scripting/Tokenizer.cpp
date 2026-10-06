#include <Error.hpp>
#include <err-types/scripting/TokenErrors.hpp>
#include <core/filesystem/dumpFile.hpp>
#include <core/scripting/tokens/Tokenizer.hpp>
#include <string_view>
#include <ankerl-hash-map/dense_map.hpp>

using namespace pg;
using namespace pg::script;

const ankerl::unordered_dense::map<std::string_view, TokenType> KeywordRegistry = {
    {"if", TokenType::IF},       {"elif", TokenType::ELIF},
    {"else", TokenType::ELSE},   {"for", TokenType::FOR},
    {"while", TokenType::WHILE}, {"return", TokenType::RETURN},
    {"exit", TokenType::EXIT},   {"as", TokenType::AS},
    {"let", TokenType::LET},     {"end", TokenType::END},
    {"done", TokenType::DONE},   {"in", TokenType::IN},
    {"exit", TokenType::EXIT},   {"use", TokenType::USE},
    {"and", TokenType::AND},     {"or", TokenType::OR},
    {"not", TokenType::NOT},     {"fn", TokenType::FUNCTION}};

// Utility functions
inline bool isWhitespace(char c) {
  switch (c) {
  case ' ':
  case '\t':
  case '\n':
    return true;
  default:
    return false;
  }
}

// If a character is reserved by the language
inline bool isReservedChar(char c) {
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

inline Token parseIdentifier(std::string_view dat, size_t &index,
                             u32 line_number) {
  auto start = index;
  for (; index < dat.length() && !isWhitespace(dat[index]) &&
         !isReservedChar(dat[index]);
       index++) {
  }
  std::string_view view{dat.substr(start, index - start)};
  index--;
  if (KeywordRegistry.contains(view))
    return {view, line_number, KeywordRegistry.at(view)};
  return {view, line_number, TokenType::IDENTIFIER};
}

inline expected<Token, pg::script::err::UnterminatedString>
parseStringLiteral(std::string_view str, size_t &index, const u32 line_num) {
  size_t start = index;
  index++;

  while (index < str.length() && str[index] != '"' && str[index] != '\n') {
    if (str[index] == '\\' && index + 1 < str.length()) {
      index += 2;
    } else {
      index++;
    }
  }
  if (index == str.length() || str[index] == '\n') {
    index--;
    return PG_UErrNew(pg::script::err::UnterminatedString,
                      .script_str =
                          std::string(str.substr(start, index + 1 - start)),
                      .line_num = line_num);
  }
  return Token{str.substr(start, index + 1 - start), line_num,
               TokenType::STRING};
}

inline expected<Token, script::err::InvalidLiteral>
parseNumericLiteral(std::string_view str, size_t &index, const u32 line_num) {
  size_t start = index;
  bool has_decimal = false;
  TokenType type = TokenType::MISSING;

  while (index < str.length()) {
    char c = str[index];

    bool breakaway = false;
    switch (c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (type != TokenType::MISSING) {
        return PG_UErrNew(
            script::err::InvalidLiteral,
            .script_str = std::string(str.substr(start, index + 1 - start)),
            .line_num = line_num,
            .err_type = script::err::InvalidLiteral::CHAR_AFTER_TYPE_SPECIFIER);
      }
      break;
    case '.':
      if (has_decimal) {
        return PG_UErrNew(
            script::err::InvalidLiteral,
            .script_str = std::string(str.substr(start, index + 1 - start)),
            .line_num = line_num,
            .err_type = script::err::InvalidLiteral::EXTRA_DECIMAL);
      } else
        has_decimal = true;
      break;
      // numeric literal type specifications below
    case 'f':
      if (!has_decimal) {
        return PG_UErrNew(
            script::err::InvalidLiteral,
            .script_str = std::string(str.substr(start, index + 1 - start)),
            .line_num = line_num,
            .err_type = script::err::InvalidLiteral::FLOAT_SPEC_BEFORE_DEMICAL);
      } else if (type != TokenType::MISSING) {
        return PG_UErrNew(
            pg::script::err::InvalidLiteral,
            .script_str = std::string(str.substr(start, index + 1 - start)),
            .line_num = line_num,
            .err_type = script::err::InvalidLiteral::MULTIPLE_TYPE_SPECIFIERS);
      } else
        type = TokenType::LITERAL_FLOAT;
      break;
    case 'u':
      if (type != TokenType::MISSING) {
        return PG_UErrNew(
            pg::script::err::InvalidLiteral,
            .script_str = std::string(str.substr(start, index + 1 - start)),
            .line_num = line_num,
            .err_type = script::err::InvalidLiteral::MULTIPLE_TYPE_SPECIFIERS);
      }
      type = TokenType::LITERAL_UINT;
      break;
    case 'i':
      if (type != TokenType::MISSING) {
        return PG_UErrNew(
            pg::script::err::InvalidLiteral,
            .script_str = std::string(str.substr(start, index + 1 - start)),
            .line_num = line_num,
            .err_type = script::err::InvalidLiteral::MULTIPLE_TYPE_SPECIFIERS);
      }
      type = TokenType::LITERAL_INT;
      break;
    default:
      if (isWhitespace(c))
        breakaway = true;
      else
        return PG_UErrNew(
            pg::script::err::InvalidLiteral,
            .script_str = std::string(str.substr(start, index + 1 - start)),
            .line_num = line_num,
            .err_type = script::err::InvalidLiteral::UNRECOGNIZED_LITERAL_CHAR);
      break;
    }
    if (breakaway)
      break; //< break out of while loop
    index++; //< otherwise keep incrementing
  }
  index--;

  if (type == TokenType::MISSING) {
    if (has_decimal)
      type = TokenType::LITERAL_FLOAT;
    else
      type = TokenType::LITERAL_INT;
  }
  return Token{std::string_view(str).substr(start, index + 1 - start), line_num,
               type};
}

inline expected<std::vector<Token>, script::err::InvalidLiteral,
                script::err::UnterminatedString>
tokenParseScript(std::string_view file) {
  std::vector<Token> tokens;
  u32 line_num = 1;

  for (size_t i = 0; i < file.length(); i++) {
    switch (file[i]) {
    // DOT
    case '.':
      if (i + 1 < file.length() &&
          std::isdigit(static_cast<unsigned char>(file[i + 1]))) {
        auto literal = parseNumericLiteral(file, i, line_num);
        PG_ReturnIfUErr(literal);
        tokens.push_back(*literal);
      } else {
        tokens.emplace_back(".", line_num, TokenType::DOT);
      }
      break;
    // Open parenthesis
    case '(':
      tokens.emplace_back("(", line_num, TokenType::OPEN_PARENTHESIS);
      break;
    // Close parenthesis
    case ')':
      tokens.emplace_back(")", line_num, TokenType::CLOSE_PARENTHESIS);
      break;
    // @
    case '@':
      tokens.emplace_back("@", line_num, TokenType::AT);
      break;
    // Equality
    case '=':
      tokens.emplace_back("=", line_num, TokenType::ASSIGN);
      break;
    // Additive
    case '+':
      tokens.emplace_back("+", line_num, TokenType::ADD);
      break;
    // Subtractive
    case '-':
      tokens.emplace_back("-", line_num, TokenType::SUB);
      break;
    // Multiplicative
    case '*':
      tokens.emplace_back("*", line_num, TokenType::MUL);
      break;
    // Division
    case '/':
      tokens.emplace_back("/", line_num, TokenType::DIV);
      break;
    // String literal
    case '"': {
      auto str_literal = parseStringLiteral(file, i, line_num);
      PG_ReturnIfUErr(str_literal);
      tokens.push_back(*str_literal);
    } break;
    // Spaces/breaks
    case '\n':
      tokens.emplace_back("\\n", line_num, TokenType::NEWLINE);
      line_num++;
      break;
    // Regular spaces
    case ' ':
    case '\t':
    case '\r':
      break;
    default:
      if (std::isdigit(static_cast<unsigned char>(file[i]))) {
        auto literal = parseNumericLiteral(file, i, line_num);
        PG_ReturnIfUErr(literal);
        tokens.push_back(*literal);
      } else {
        tokens.push_back(parseIdentifier(file, i, line_num));
      }
      break;
    }
  }
  tokens.emplace_back("EOF", line_num, TokenType::END_OF_FILE);
  return tokens;
}

expected<Tokenizer, script::err::InvalidLiteral,
         script::err::UnterminatedString, pg::err::FileNotExists,
         pg::err::FileNotOpened>
Tokenizer::make(const pg::path &p) {
  auto script_exp = pg::dumpFile(p);
  PG_ReturnIfUErr(script_exp);
  auto token_vec = tokenParseScript(*script_exp);
  PG_ReturnIfUErr(token_vec);
  return Tokenizer(std::move(*script_exp), std::move(*token_vec));
}

Tokenizer::Tokenizer(std::string &&str, std::vector<Token> &&tokens)
    : script(std::move(str)), tokens(std::move(tokens)) {}
