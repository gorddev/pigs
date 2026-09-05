#pragma once

#include <string>
#include <toolkit/intdef.h>


namespace pg::script::err {

	struct InvalidLiteral {

		std::string number_str;
		u32 line_number;
		enum ErrType : char {
			EXTRA_DECIMAL
		} err_type;
		std::string str() const {
			return std::string(err_type_str(err_type)) + " (Line: " + std::to_string(line_number) + ")";
		}

	private:
		const char* err_type_str(ErrType t) const {
			switch (t) {
				case EXTRA_DECIMAL:
					return "Could not parse number because of an extra decimal place.";
			}
			return "Unknown Invalid Literal Error";
		}
	};

}
