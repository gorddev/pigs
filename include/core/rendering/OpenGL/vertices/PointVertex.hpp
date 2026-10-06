#pragma once

#include "Is_Vertex.hpp"

namespace pg {

	/** (Point Vertex): Barebones vertex with three position attributes (x,y,z) and
	 * nothing else. Intended to be used with fragment shaders for various purposes.
	 */
	struct PointVertex {
	  using Self = PointVertex;
	  static constexpr size_t num_attributes = 1;
	  float x, y, z; //< position data

	  static void setup_attributes() {
	    // setting up (x,y,z)
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Self),
	                          reinterpret_cast<void *>(offsetof(Self, x)));
	    glEnableVertexAttribArray(0);
	  }
	  // —————————————————————————— //

	  static_assert(Is_Vertex<Self>);
	};

	using PVertex = PointVertex;

}
