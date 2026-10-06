#pragma once

#include "LeafNodes.hpp"
#include "OpNodes.hpp"
#include "ScopeNodes.hpp"
#include "StatementNodes.hpp"
#include <core/errors/unwrap.hpp>
#include <core/scripting/tokens/Token.hpp>
#include <core/scripting/tokens/TokenInfo.hpp>
#include <core/scripting/tokens/TokenPrecedence.hpp>
#include <vector>
#include <iostream>

namespace pg::script {

class Parser {
public:
  explicit Parser(std::span<const Token> tokens) : tokens_(tokens) {}

  std::unique_ptr<ProgramNode> parse() {
    auto program = std::make_unique<ProgramNode>();

    skipNewlines();

    while (!atEnd()) {
      program->statements.push_back(parseStatement());

      skipNewlines();
    }

    return program;
  }

private:
  std::span<const Token> tokens_;
  std::size_t current_ = 0;

private:
  const Token &peek() const { return tokens_[current_]; }

  const Token &previous() const { return tokens_[current_ - 1]; }

  bool atEnd() const { return peek().type == TokenType::END_OF_FILE; }

  const Token &advance() {
    if (!atEnd())
      ++current_;

    return previous();
  }

  bool check(TokenType type) const { return peek().type == type; }

  bool match(TokenType type) {
    if (!check(type))
      return false;

    advance();
    return true;
  }

  void expect(TokenType type) {
    if (!match(type)) {
      PG_Panic("unexpected token at line ", std::to_string(peek().line));
    }
  }

  void skipNewlines() {
    while (match(TokenType::NEWLINE))
      ;
  }

  ASTPtr parseExpression(TokenPrecedence minPrecedence = PRECEDENCE_NONE) {
    ASTPtr lhs = parsePrefix();

    while (!atEnd()) {
      TokenType op = peek().type;
      TokenInfo info = getTokenInfo(op);

      if (op == TokenType::NEWLINE || op == TokenType::END ||
          op == TokenType::DONE) {
        break;
      }

      if (info.classification != TokenClassification::BINARY)
        break;

      if (info.precedence < minPrecedence)
        break;

      advance();

      lhs = parseInfix(op, std::move(lhs), info.precedence);
    }

    return lhs;
  }

  ASTPtr parseCall(ASTPtr callee) {
    auto call = std::make_unique<CallNode>(std::move(callee), previous().line);

    if (!check(TokenType::CLOSE_PARENTHESIS)) {
      do {
        call->arguments.push_back(parseExpression());
      } while (match(TokenType::NEWLINE) == false &&
               match(TokenType::CLOSE_PARENTHESIS) == false);

      if (!check(TokenType::CLOSE_PARENTHESIS))
        expect(TokenType::CLOSE_PARENTHESIS);
    } else {
      advance();
    }

    return call;
  }

  ASTPtr parsePrefix() {
      const Token token = advance();

      switch (token.type) {

      case TokenType::IDENTIFIER:
          return std::make_unique<IdentifierNode>(token.str, token.line);

      case TokenType::AT: {
          if (!check(TokenType::IDENTIFIER)) {
              PG_Panic("expected builtin function name after '@' at line ",
                       std::to_string(peek().line));
          }

          Token builtin = advance();

          return std::make_unique<CallNode>(
              std::make_unique<IdentifierNode>(
                  builtin.str,
                  builtin.line
              ),
              builtin.line
          );
      }

      case TokenType::STRING:
          return std::make_unique<StringNode>(token.str, token.line);

      case TokenType::LITERAL_INT:
          return std::make_unique<IntNode>(token.str, token.line);

      case TokenType::LITERAL_UINT:
          return std::make_unique<UIntNode>(token.str, token.line);

      case TokenType::LITERAL_FLOAT:
          return std::make_unique<FloatNode>(token.str, token.line);

      case TokenType::NOT:
      case TokenType::SUB:
      case TokenType::ADD: {
          auto operand = parseExpression(PREFIX);

          return std::make_unique<UnaryNode>(
              token.type,
              std::move(operand),
              token.line
          );
      }

      case TokenType::OPEN_PARENTHESIS: {
          auto expression = parseExpression();

          expect(TokenType::CLOSE_PARENTHESIS);

          return expression;
      }

      default:
          PG_Panic(
              "expected expression at line " +
              std::to_string(token.line)
          );
      }
  }


  ASTPtr parseInfix(TokenType op, ASTPtr lhs, TokenPrecedence precedence) {
    switch (op) {

    case TokenType::ADD:
    case TokenType::SUB:
    case TokenType::MUL:
    case TokenType::DIV:
    case TokenType::AND:
    case TokenType::OR: {
      auto rhs = parseExpression(static_cast<TokenPrecedence>(precedence + 1));

      return std::make_unique<BinaryNode>(op, std::move(lhs), std::move(rhs),
                                          previous().line);
    }

    case TokenType::ASSIGN: {
      auto rhs = parseExpression(ASSIGNMENT);

      return std::make_unique<AssignmentNode>(std::move(lhs), std::move(rhs),
                                              previous().line);
    }

    case TokenType::DOT: {
      if (!check(TokenType::IDENTIFIER)) {
        PG_Panic("expected identifier after '.'");
      }

      Token member = advance();

      return std::make_unique<MemberAccessNode>(std::move(lhs), member.str,
                                                member.line);
    }

    case TokenType::OPEN_PARENTHESIS:
      return parseCall(std::move(lhs));

    default:
      PG_Panic("invalid infix operator");
    }
  }

  ASTPtr parseStatement() {
    const Token &token = peek();

    switch (token.type) {
    case TokenType::LET:
      return parseLet();

    case TokenType::RETURN:
      return parseReturn();

    case TokenType::IF:
      return parseIf();

    case TokenType::WHILE:
      return parseWhile();

    case TokenType::FOR:
      return parseFor();

    case TokenType::USE:
      return parseUse();

    case TokenType::EXIT:
      return parseExit();

    default:
      return parseExpressionStatement();
    }
  }

  ASTPtr parseExpressionStatement() {
    const u32 line = peek().line;

    auto expression = parseExpression();

    // The expression must end here.
    if (!check(TokenType::NEWLINE) && !check(TokenType::END_OF_FILE) &&
        !check(TokenType::END) && !check(TokenType::DONE)) {
      PG_Panic("unexpected token after expression at line ",
               std::to_string(peek().line));
    }

    match(TokenType::NEWLINE);

    return std::make_unique<ExpressionStatementNode>(std::move(expression),
                                                     line);
  }

  ASTPtr parseLet() {
    Token letToken = advance();

    if (!check(TokenType::IDENTIFIER)) {
      PG_Panic("expected identifier after 'let'");
    }

    Token name = advance();

    ASTPtr value = nullptr;

    if (match(TokenType::ASSIGN)) {
      value = parseExpression();
    }

    if (!check(TokenType::NEWLINE) && !check(TokenType::END_OF_FILE)) {
      PG_Panic("unexpected token after let declaration");
    }

    match(TokenType::NEWLINE);

    return std::make_unique<LetNode>(name.str, std::move(value), letToken.line);
  }

  ASTPtr parseReturn() {
    Token returnToken = advance();

    ASTPtr value = nullptr;

    // `return` by itself is valid.
    if (!check(TokenType::NEWLINE) && !check(TokenType::END_OF_FILE) &&
        !check(TokenType::END) && !check(TokenType::DONE)) {
      value = parseExpression();
    }

    match(TokenType::NEWLINE);

    return std::make_unique<ReturnNode>(std::move(value), returnToken.line);
  }

  std::unique_ptr<BlockNode> parseBlock() {
    auto block = std::make_unique<BlockNode>(peek().line);

    skipNewlines();

    while (!atEnd() && !check(TokenType::END) && !check(TokenType::DONE)) {
      block->statements.push_back(parseStatement());

      skipNewlines();
    }

    return block;
  }

  ASTPtr parseIf() {
    Token ifToken = advance();

    auto condition = parseExpression();

    // `if <expression>` must end at newline.
    expect(TokenType::NEWLINE);

    auto body = parseBlock();

    expect(TokenType::END);

    // END may itself be followed by newline.
    match(TokenType::NEWLINE);

    return std::make_unique<IfNode>(std::move(condition), std::move(body),
                                    ifToken.line);
  }

  ASTPtr parseWhile() {
    Token whileToken = advance();

    auto condition = parseExpression();

    expect(TokenType::NEWLINE);

    auto body = parseBlock();

    expect(TokenType::END);
    match(TokenType::NEWLINE);

    return std::make_unique<WhileNode>(std::move(condition), std::move(body),
                                       whileToken.line);
  }

  ASTPtr parseFor() {
    Token forToken = advance();

    auto value = parseExpression();

    expect(TokenType::NEWLINE);

    auto body = parseBlock();

    expect(TokenType::END);
    match(TokenType::NEWLINE);

    return std::make_unique<ForNode>(std::move(value), std::move(body),
                                     forToken.line);
  }

  ASTPtr parseUse() {
      Token useToken = advance();

      auto value = parseExpression();

      std::string_view alias{};

      if (match(TokenType::AS)) {
          if (!check(TokenType::IDENTIFIER)) {
              PG_Panic(
                  "expected identifier after 'as' at line " +
                  std::to_string(peek().line)
              );
          }

          alias = advance().str;
      }

      // A use statement must end here.
      if (!check(TokenType::NEWLINE) &&
          !check(TokenType::END_OF_FILE))
      {
          PG_Panic(
              "unexpected token after 'use' at line ",
              std::to_string(peek().line)
          );
      }

      match(TokenType::NEWLINE);

      return std::make_unique<UseNode>(
          std::move(value),
          alias,
          useToken.line
      );
  }


  ASTPtr parseExit() {
    Token exitToken = advance();

    // exit <expression>
    auto value = parseExpression();

    if (!check(TokenType::NEWLINE) && !check(TokenType::END_OF_FILE)) {
      PG_Panic("unexpected token after 'exit' at line ",
               std::to_string(peek().line));
    }

    match(TokenType::NEWLINE);

    return std::make_unique<ExitNode>(std::move(value), exitToken.line);
  }
};

#include <string_view>

class ASTPrinter {
public:
    static void print(const ASTNode& node) {
        printNode(node, "", true);
    }

private:
    static void printNode(
        const ASTNode& node,
        std::string_view prefix,
        bool last)
    {
        std::cout << prefix
                  << (last ? "└── " : "├── ");

        printLabel(node);

        std::cout << '\n';

        std::string childPrefix(prefix);

        if (last)
            childPrefix += "    ";
        else
            childPrefix += "│   ";

        printChildren(node, childPrefix);
    }

    static void printLabel(const ASTNode& node) {
        switch (node.kind) {
            case ASTNode::Kind::Program:
                std::cout << "Program";
                break;

            case ASTNode::Kind::Block:
                std::cout << "Block";
                break;

            case ASTNode::Kind::Identifier: {
                auto& n =
                    static_cast<const IdentifierNode&>(node);

                std::cout << "Identifier: " << n.name;
                break;
            }

            case ASTNode::Kind::String: {
                auto& n =
                    static_cast<const StringNode&>(node);

                std::cout << "String: \"" << n.value << "\"";
                break;
            }

            case ASTNode::Kind::Int: {
                auto& n =
                    static_cast<const IntNode&>(node);

                std::cout << "Int: " << n.value;
                break;
            }

            case ASTNode::Kind::UInt: {
                auto& n =
                    static_cast<const UIntNode&>(node);

                std::cout << "UInt: " << n.value;
                break;
            }

            case ASTNode::Kind::Float: {
                auto& n =
                    static_cast<const FloatNode&>(node);

                std::cout << "Float: " << n.value;
                break;
            }

            case ASTNode::Kind::Unary: {
                auto& n =
                    static_cast<const UnaryNode&>(node);

                std::cout << "Unary: "
                          << tokenName(n.op);
                break;
            }

            case ASTNode::Kind::Binary: {
                auto& n =
                    static_cast<const BinaryNode&>(node);

                std::cout << "Binary: "
                          << tokenName(n.op);
                break;
            }

            case ASTNode::Kind::Call:
                std::cout << "Call";
                break;

            case ASTNode::Kind::MemberAccess: {
                auto& n =
                    static_cast<const MemberAccessNode&>(node);

                std::cout << "Member: "
                          << n.member;
                break;
            }

            case ASTNode::Kind::ExpressionStatement:
                std::cout << "ExpressionStatement";
                break;

            case ASTNode::Kind::Assignment:
                std::cout << "Assignment";
                break;

            case ASTNode::Kind::Let: {
                auto& n =
                    static_cast<const LetNode&>(node);

                std::cout << "Let: " << n.name;
                break;
            }

            case ASTNode::Kind::Return:
                std::cout << "Return";
                break;

            case ASTNode::Kind::If:
                std::cout << "If";
                break;

            case ASTNode::Kind::While:
                std::cout << "While";
                break;

            case ASTNode::Kind::For:
                std::cout << "For";
                break;

            case ASTNode::Kind::Use:
                std::cout << "Use";
                break;

            case ASTNode::Kind::Exit:
                std::cout << "Exit";
                break;
        }
    }

    static void printChildren(
        const ASTNode& node,
        std::string_view prefix)
    {
        switch (node.kind) {
            case ASTNode::Kind::Program: {
                auto& n =
                    static_cast<const ProgramNode&>(node);

                printVector(n.statements, prefix);
                break;
            }

            case ASTNode::Kind::Block: {
                auto& n =
                    static_cast<const BlockNode&>(node);

                printVector(n.statements, prefix);
                break;
            }

            case ASTNode::Kind::Unary: {
                auto& n =
                    static_cast<const UnaryNode&>(node);

                printChild("operand", n.operand, prefix, true);
                break;
            }

            case ASTNode::Kind::Binary: {
                auto& n =
                    static_cast<const BinaryNode&>(node);

                printChild("lhs", n.lhs, prefix, false);
                printChild("rhs", n.rhs, prefix, true);
                break;
            }

            case ASTNode::Kind::Assignment: {
                auto& n =
                    static_cast<const AssignmentNode&>(node);

                printChild("lhs", n.lhs, prefix, false);
                printChild("rhs", n.rhs, prefix, true);
                break;
            }

            case ASTNode::Kind::ExpressionStatement: {
                auto& n =
                    static_cast<const ExpressionStatementNode&>(node);

                printChild(
                    "expression",
                    n.expression,
                    prefix,
                    true
                );
                break;
            }

            case ASTNode::Kind::Call: {
                auto& n =
                    static_cast<const CallNode&>(node);

                printChild(
                    "callee",
                    n.callee,
                    prefix,
                    n.arguments.empty()
                );

                for (std::size_t i = 0;
                     i < n.arguments.size();
                     ++i)
                {
                    bool last =
                        i == n.arguments.size() - 1;

                    printChild(
                        "argument",
                        n.arguments[i],
                        prefix,
                        last
                    );
                }

                break;
            }

            case ASTNode::Kind::MemberAccess: {
                auto& n =
                    static_cast<const MemberAccessNode&>(node);

                printChild(
                    "object",
                    n.object,
                    prefix,
                    true
                );
                break;
            }

            case ASTNode::Kind::Let: {
                auto& n =
                    static_cast<const LetNode&>(node);

                if (n.value) {
                    printChild(
                        "value",
                        n.value,
                        prefix,
                        true
                    );
                }

                break;
            }

            case ASTNode::Kind::Return: {
                auto& n =
                    static_cast<const ReturnNode&>(node);

                if (n.value) {
                    printChild(
                        "value",
                        n.value,
                        prefix,
                        true
                    );
                }

                break;
            }

            case ASTNode::Kind::If: {
                auto& n =
                    static_cast<const IfNode&>(node);

                printChild(
                    "condition",
                    n.condition,
                    prefix,
                    false
                );

                printChild(
                    "body",
                    n.body,
                    prefix,
                    true
                );

                break;
            }

            case ASTNode::Kind::While: {
                auto& n =
                    static_cast<const WhileNode&>(node);

                printChild(
                    "condition",
                    n.condition,
                    prefix,
                    false
                );

                printChild(
                    "body",
                    n.body,
                    prefix,
                    true
                );

                break;
            }

            case ASTNode::Kind::For: {
                auto& n =
                    static_cast<const ForNode&>(node);

                printChild(
                    "value",
                    n.value,
                    prefix,
                    false
                );

                printChild(
                    "body",
                    n.body,
                    prefix,
                    true
                );

                break;
            }

            case ASTNode::Kind::Use: {
                auto& n =
                    static_cast<const UseNode&>(node);

                printChild(
                    "value",
                    n.value,
                    prefix,
                    true
                );

                break;
            }

            case ASTNode::Kind::Exit: {
                auto& n =
                    static_cast<const ExitNode&>(node);

                printChild(
                    "value",
                    n.value,
                    prefix,
                    true
                );

                break;
            }

            // Leaf nodes have no children.
            case ASTNode::Kind::Identifier:
            case ASTNode::Kind::String:
            case ASTNode::Kind::Int:
            case ASTNode::Kind::UInt:
            case ASTNode::Kind::Float:
                break;
        }
    }

    template <typename T>
    static void printChild(
        std::string_view name,
        const std::unique_ptr<T>& child,
        std::string_view prefix,
        bool last)
    {
        std::cout << prefix
                  << (last ? "└── " : "├── ")
                  << name
                  << '\n';

        std::string childPrefix(prefix);
        childPrefix += last ? "    " : "│   ";

        if (child)
            printNode(*child, childPrefix, true);
    }


    static void printVector(
        const std::vector<ASTPtr>& nodes,
        std::string_view prefix)
    {
        for (std::size_t i = 0; i < nodes.size(); ++i) {
            printNode(
                *nodes[i],
                prefix,
                i == nodes.size() - 1
            );
        }
    }

    static const char* tokenName(TokenType type) {
        switch (type) {
            case TokenType::ASSIGN: return "=";
            case TokenType::ADD:    return "+";
            case TokenType::SUB:    return "-";
            case TokenType::MUL:    return "*";
            case TokenType::DIV:    return "/";
            case TokenType::AND:    return "and";
            case TokenType::OR:     return "or";
            case TokenType::NOT:    return "not";
            case TokenType::DOT:    return ".";
            default:                return "?";
        }
    }
};

} // namespace pg::script
