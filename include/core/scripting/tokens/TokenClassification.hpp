#pragma once

namespace pg::script {

	enum class TokenClassification : char {
		UNARY,
		BINARY,
		SOLO,
		LEAF,
		NO_CLASSIFICATION
	};

}
