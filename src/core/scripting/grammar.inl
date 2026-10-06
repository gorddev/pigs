#ifndef PG_SCRIPT_GRAMMAR_INL
#define PG_SCRIPT_GRAMMAR_INL

#include <core/scripting/scriptdef.hpp>

namespace pg::inl {

bool isPrefixToken(pg::script::TokenType type) {
	using namespace pg::script;
    switch (type) {
        // Leaves / Literals
        case TokenType::IDENTIFIER:
        case TokenType::LITERAL_INT:
        case TokenType::LITERAL_FLOAT:
        case TokenType::LITERAL_UINT:
        case TokenType::STRING:
        // True Prefix Operators
        case TokenType::NOT:
        case TokenType::SUB:
        case TokenType::OPEN_PARENTHESIS:
            return true;
        default:
            return false;
    }
}

bool isInfixToken

}


#endif
