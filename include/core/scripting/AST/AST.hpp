#pragma once

#include <toolkit/intdef.h>
#include <memory>

namespace pg::script {

struct ASTNode {
  enum class Kind : char {
    Identifier,
    String,
    Int,
    UInt,
    Float,

    Unary,
    Binary,
    Call,
    MemberAccess,

    ExpressionStatement,
    Assignment,
    Let,
    Return,
    If,
    While,
    For,
    Use,
    Exit,

    Program,
    Block
  };

  u32 line;
  Kind kind;

  explicit ASTNode(Kind kind, u32 line) : kind(kind), line(line) {}

  virtual ~ASTNode() = default;
};

using ASTPtr = std::unique_ptr<ASTNode>;

}
