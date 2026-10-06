#pragma once

#include <toolkit/apidef.h>
#include <cstdint>
#include <cstddef>
#include <type_traits>

namespace pg {
	/** Is_Vertex requires that a vertex struct have an @code static void
	 * attribute()@endcode function that properly sets up the attribute pointers for
	 * OpenGL shaders */
	template <typename V>
	concept Is_Vertex = requires(V v) {
	  std::is_same_v<decltype(V::setup_attributes()), void>;
	  std::is_same_v<decltype(V::num_attributes), const size_t>;
	};
}
