#pragma once

namespace pg::script {

	enum TokenPrecedence : char {
    PRECEDENCE_NONE,

    ASSIGNMENT,
    DIRECTIVE,
    LOGICAL,
    ARITHMETIC,
    MULTIPLICATIVE,

    PREFIX,

    DOT,
    CALL
	};

}
