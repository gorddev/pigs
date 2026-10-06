#include "AST.hpp"

#include <string_view>

namespace pg::script {

	struct IdentifierNode final : ASTNode {
	    std::string_view name;

	    IdentifierNode(std::string_view name, u32 line)
	        : ASTNode(Kind::Identifier, line),
	          name(name) {}
	};

	struct StringNode final : ASTNode {
	    std::string_view value;

	    StringNode(std::string_view value, u32 line)
	        : ASTNode(Kind::String, line),
	          value(value) {}
	};

	struct IntNode final : ASTNode {
	    std::string_view value;

	    IntNode(std::string_view value, u32 line)
	        : ASTNode(Kind::Int, line),
	          value(value) {}
	};

	struct UIntNode final : ASTNode {
	    std::string_view value;

	    UIntNode(std::string_view value, u32 line)
	        : ASTNode(Kind::UInt, line),
	          value(value) {}
	};

	struct FloatNode final : ASTNode {
	    std::string_view value;

	    FloatNode(std::string_view value, u32 line)
	        : ASTNode(Kind::Float, line),
	          value(value) {}
	};
}
