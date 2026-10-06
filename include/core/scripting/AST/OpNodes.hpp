#pragma once


#include "AST.hpp"
#include <core/scripting/tokens/TokenType.hpp>


namespace pg::script {

	struct UnaryNode final : ASTNode {
    TokenType op;
    ASTPtr operand;

    UnaryNode(TokenType op, ASTPtr operand, u32 line)
        : ASTNode(Kind::Unary, line),
          op(op),
          operand(std::move(operand)) {}
	};

	struct BinaryNode final : ASTNode {
    TokenType op;
    ASTPtr lhs;
    ASTPtr rhs;

    BinaryNode(TokenType op, ASTPtr lhs, ASTPtr rhs, u32 line)
        : ASTNode(Kind::Binary, line),
          op(op),
          lhs(std::move(lhs)),
          rhs(std::move(rhs)) {}
	};


}
