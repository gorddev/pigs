#pragma once

#include "AST.hpp"
#include <vector>


namespace pg::script {

	struct CallNode final : ASTNode {
    ASTPtr callee;
    std::vector<ASTPtr> arguments;

    CallNode(ASTPtr callee, u32 line)
        : ASTNode(Kind::Call, line),
          callee(std::move(callee)) {}
	};

	struct MemberAccessNode final : ASTNode {
    ASTPtr object;
    std::string_view member;

    MemberAccessNode(ASTPtr object,
                     std::string_view member,
                     u32 line)
        : ASTNode(Kind::MemberAccess, line),
          object(std::move(object)),
          member(member) {}
	};





}
