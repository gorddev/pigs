#pragma once


#include <string>
#include <type_traits>

static_assert(std::is_convertible_v<const char[2], const char*>);

namespace pg::err {
	template<typename...Args>
	std::string errStringFormatter(Args&&... args) {
		std::string ret;
		(([&ret](const auto& val) {
			using val_t = std::remove_cvref<decltype(val)>::type;
			if constexpr(std::is_same_v<val_t, std::string>)
				ret += val;
			else if constexpr(std::is_convertible_v<val_t, const char*>)
				ret += val;
			else if constexpr(!std::is_convertible_v<val_t, const char*>)
				ret += std::to_string(val);
		})(args), ...);
		return ret;
	}
}

#define PG_ErrStruct(struct_name, member_vars, ...)\
	struct struct_name {\
		member_vars\
		std::string str() const {\
			return pg::err::errStringFormatter(__VA_ARGS__);\
		}\
	};
