
#pragma once

#include <core/scripting/Parser.hpp>
#include <core/scripting/Tokenizer.hpp>
#include <memory>
#include <vector>

#include "grammar.inl"

using namespace pg;
using namespace pg::script;



#include <array>
#include <memory>
#include <toolkit/intdef.h>

namespace pg::script {

// Forward declarations for clean compile separation

class Parser;

struct ASTNode {
  virtual ~ASTNode() = default;
};

struct LeafNode : public ASTNode {
  Token token;
};

struct UnaryExpressionNode : public ASTNode {
  Token op;
  std::unique_ptr<ASTNode> right;
};

struct BinaryExpressionNode : public ASTNode {
  std::unique_ptr<ASTNode> left;
  Token op;
  std::unique_ptr<ASTNode> right;
};


using NudFunction = std::unique_ptr<ASTNode> (Parser::*)();
using LedFunction =
    std::unique_ptr<ASTNode> (Parser::*)(std::unique_ptr<ASTNode> left);

struct ParseRule {
  NudFunction nud;
  LedFunction led;
  TokenPrecedence precedence;
};

class ParseTable {
private:
  static constexpr size_t TableSize =
      static_cast<size_t>(TokenType::END_OF_FILE) + 1;
  std::array<ParseRule, TableSize> rules;

public:
  ParseTable();
  inline const ParseRule &operator[](TokenType type) const {
    return rules[static_cast<size_t>(type)];
  }
};

}

TokenPrecedence tokenToPrecedence(const Token &t) {
  switch (t.type) {
  case TokenType::IF:
  case TokenType::ELIF:
  case TokenType::ELSE:
  case TokenType::WHILE:
  case TokenType::RETURN:
  case TokenType::AS:
  case TokenType::LET:
  case TokenType::USE:
  case TokenType::IN:
  case TokenType::EXIT:
  case TokenType::NOT:
  case TokenType::FOR:
  case TokenType::AT:
    return TokenPrecedence::DIRECTIVE;
  case TokenType::END:
  case TokenType::DONE:
  case TokenType::FUNCTION:
    return TokenPrecedence::CALL;
  case TokenType::ASSIGN:
    return TokenPrecedence::ASSIGNMENT;
  case TokenType::ADD:
  case TokenType::SUB:
    return TokenPrecedence::ARITHMETIC;
  case TokenType::MUL:
  case TokenType::DIV:
    return TokenPrecedence::MULTIPLICATIVE;
  case TokenType::AND:
  case TokenType::OR:
    return TokenPrecedence::LOGICAL;
  case TokenType::DOT:
    return TokenPrecedence::DOT;
  case TokenType::OPEN_PARENTHESIS:
  case TokenType::CLOSE_PARENTHESIS:
    return TokenPrecedence::PARENTHESIS;
  case TokenType::IDENTIFIER:
  case TokenType::LITERAL_INT:
  case TokenType::LITERAL_FLOAT:
  case TokenType::LITERAL_UINT:
  case TokenType::MISSING:
  case TokenType::STRING:
  case TokenType::NEWLINE:
  case TokenType::END_OF_FILE:
    return TokenPrecedence::PRECEDENCE_NONE;
  }
}

TokenClassification tokenToClassification(const Token &t) {
  switch (t.type) {
  case TokenType::IF:
  case TokenType::ELIF:
  case TokenType::ELSE:
  case TokenType::WHILE:
  case TokenType::RETURN:
  case TokenType::AS:
  case TokenType::LET:
  case TokenType::USE:
  case TokenType::IN:
  case TokenType::EXIT:
  case TokenType::NOT:
  case TokenType::FUNCTION:
  case TokenType::FOR:
  case TokenType::AT:
    return TokenClassification::UNARY;
  case TokenType::END:
  case TokenType::DONE:
    return TokenClassification::SOLO;
  case TokenType::ASSIGN:
  case TokenType::ADD:
  case TokenType::SUB:
  case TokenType::MUL:
  case TokenType::DIV:
  case TokenType::AND:
  case TokenType::OR:
  case TokenType::DOT:
    return TokenClassification::BINARY;
  case TokenType::IDENTIFIER:
  case TokenType::LITERAL_INT:
  case TokenType::LITERAL_FLOAT:
  case TokenType::LITERAL_UINT:
  case TokenType::STRING:
    return TokenClassification::LEAF;
  case TokenType::OPEN_PARENTHESIS:
  case TokenType::CLOSE_PARENTHESIS:
  case TokenType::MISSING:
  case TokenType::NEWLINE:
  case TokenType::END_OF_FILE:
    return TokenClassification::NO_CLASSIFICATION;
  };
}

void parse(const Tokenizer &tk) {
  std::vector<std::unique_ptr<ASTNode>> expressions;

  while (!tk.eof()) {
  }
}
