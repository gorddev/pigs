#pragma once

#include "Is_Vertex.hpp"

namespace pg {
	/** (Color Vertex): Vertex with positions and color. */
	struct ColorVertex {
	  using Self = ColorVertex;
	  static constexpr size_t num_attributes = 2;

	  float x, y, z;    //< position data
	  float r, g, b, a; //< color;

	  static void setup_attributes() {
	    // setting up (x,y,z)
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Self),
	      reinterpret_cast<void *>(offsetof(Self, x)));
	    glEnableVertexAttribArray(0);
	    // setting up (x,y,z)
	    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Self),
	      reinterpret_cast<void *>(offsetof(Self, r)));
	    glEnableVertexAttribArray(1);
	  }
	  // —————————————————————————— //

	  static_assert(Is_Vertex<Self>);
	};

	using CVetex = ColorVertex;
}
