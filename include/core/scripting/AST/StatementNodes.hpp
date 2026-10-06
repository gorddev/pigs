#pragma once

#include "AST.hpp"
#include <string_view>
#include <vector>

namespace pg::script {

	struct ExpressionStatementNode final : ASTNode {
    ASTPtr expression;

    ExpressionStatementNode(ASTPtr expression, u32 line)
        : ASTNode(Kind::ExpressionStatement, line),
          expression(std::move(expression)) {}
	};

	struct AssignmentNode final : ASTNode {
    ASTPtr lhs;
    ASTPtr rhs;

    AssignmentNode(ASTPtr lhs, ASTPtr rhs, u32 line)
        : ASTNode(Kind::Assignment, line),
          lhs(std::move(lhs)),
          rhs(std::move(rhs)) {}
	};

	struct LetNode final : ASTNode {
    std::string_view name;
    ASTPtr value;

    LetNode(std::string_view name, ASTPtr value, u32 line)
        : ASTNode(Kind::Let, line),
          name(name),
          value(std::move(value)) {}
	};

	struct ReturnNode final : ASTNode {
    ASTPtr value;

    ReturnNode(ASTPtr value, u32 line)
        : ASTNode(Kind::Return, line),
          value(std::move(value)) {}
	};

	struct BlockNode final : ASTNode {
    std::vector<ASTPtr> statements;

    explicit BlockNode(u32 line)
        : ASTNode(Kind::Block, line) {}
	};

	struct IfNode final : ASTNode {
    ASTPtr condition;
    std::unique_ptr<BlockNode> body;

    IfNode(ASTPtr condition,
           std::unique_ptr<BlockNode> body,
           u32 line)
        : ASTNode(Kind::If, line),
          condition(std::move(condition)),
          body(std::move(body)) {}
	};

	struct WhileNode final : ASTNode {
    ASTPtr condition;
    std::unique_ptr<BlockNode> body;

    WhileNode(ASTPtr condition,
              std::unique_ptr<BlockNode> body,
              u32 line)
        : ASTNode(Kind::While, line),
          condition(std::move(condition)),
          body(std::move(body)) {}
	};

	struct ProgramNode final : ASTNode {
    std::vector<ASTPtr> statements;

    ProgramNode()
        : ASTNode(Kind::Program, 0) {}
	};

	struct ForNode final : ASTNode {
    ASTPtr value;
    std::unique_ptr<BlockNode> body;

    ForNode(ASTPtr value,
            std::unique_ptr<BlockNode> body,
            u32 line)
        : ASTNode(Kind::For, line),
          value(std::move(value)),
          body(std::move(body)) {}
	};

	struct UseNode final : ASTNode {
    ASTPtr value;
    std::string_view alias;

    UseNode(
        ASTPtr value,
        std::string_view alias,
        u32 line
    )
        : ASTNode(Kind::Use, line),
          value(std::move(value)),
          alias(alias) {}
	};

	struct ExitNode final : ASTNode {
    ASTPtr value;

    ExitNode(ASTPtr value, u32 line)
        : ASTNode(Kind::Exit, line),
          value(std::move(value)) {}
	};

}
