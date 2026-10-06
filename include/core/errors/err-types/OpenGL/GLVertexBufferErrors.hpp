#pragma once

#include "toolkit/intdef.h"
#include <string>
namespace pg::err {

	struct GLMatrixBufferInvalidShaderLoc {
		size_t provided_value;
		size_t num_attributes;
		std::string str() const {
			return std::string("Shader location parameter must be divisible by 4 and greater than ") +
				std::to_string(num_attributes) + ". (Provided value: " + std::to_string(provided_value) + ')';
		}
	};

	struct GLColorBufferInvalidShaderLoc {
		size_t provided_value;
		size_t num_attributes;
		std::string str() const {
			return std::string("Shader location must be greater than ") + std::to_string(num_attributes) +
				" as to not overwrite attribute pointers. (Provided value: " + std::to_string(provided_value) + ')';
		}
	};

}
