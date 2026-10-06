#pragma once

#include "TokenClassification.hpp"
#include "TokenPrecedence.hpp"
#include "TokenType.hpp"

namespace pg::script {
struct TokenInfo {
  TokenPrecedence precedence;
  TokenClassification classification;
};

inline constexpr TokenInfo getTokenInfo(TokenType type) {
  switch (type) {
  // Binary
  case TokenType::ASSIGN:
    return {ASSIGNMENT, TokenClassification::BINARY};

  case TokenType::OR:
  case TokenType::AND:
    return {LOGICAL, TokenClassification::BINARY};

  case TokenType::ADD:
  case TokenType::SUB:
    return {ARITHMETIC, TokenClassification::BINARY};

  case TokenType::MUL:
  case TokenType::DIV:
    return {MULTIPLICATIVE, TokenClassification::BINARY};

  // Prefix
  case TokenType::NOT:
    return {PREFIX, TokenClassification::UNARY};

  // Call/member access
  case TokenType::DOT:
    return {DOT, TokenClassification::BINARY};

  case TokenType::OPEN_PARENTHESIS:
    return {CALL, TokenClassification::BINARY};

  // Leaves
  case TokenType::IDENTIFIER:
  case TokenType::STRING:
  case TokenType::LITERAL_INT:
  case TokenType::LITERAL_UINT:
  case TokenType::LITERAL_FLOAT:
    return {PRECEDENCE_NONE, TokenClassification::LEAF};

  // Unary keywords
  case TokenType::IF:
  case TokenType::ELIF:
  case TokenType::ELSE:
  case TokenType::FOR:
  case TokenType::WHILE:
  case TokenType::RETURN:
  case TokenType::AS:
  case TokenType::LET:
  case TokenType::USE:
  case TokenType::IN:
  case TokenType::EXIT:
    return {PREFIX, TokenClassification::UNARY};

  // Statement terminators
  case TokenType::END:
  case TokenType::DONE:
  case TokenType::NEWLINE:
  case TokenType::END_OF_FILE:
    return {PRECEDENCE_NONE, TokenClassification::SOLO};

  default:
    return {PRECEDENCE_NONE, TokenClassification::NO_CLASSIFICATION};
  }
}

} // namespace pg::script
