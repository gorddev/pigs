#pragma once


#include <toolkit/apidef.h>
#include <cstddef>
#include "Is_Vertex.hpp"


namespace pg{

	/** Standard vertex with a total of 3 attributes:
	 * - @code float x, y, z;@endcode       > position
	 * - @code float u, v;@endcode          > texture position
	 * - @code float nx, ny, nz:@endcode    > normalized coords */
	struct Vertex {
  using Self = Vertex;

  float x, y, z;
  float u, v;
  float nx, ny, nz;

  static constexpr size_t num_attributes = 3;

  static void setup_attributes() {
    // x, y, z positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Self),
      reinterpret_cast<void *>(offsetof(Self, x)));
    glEnableVertexAttribArray(0);
    // u, v coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Self),
      reinterpret_cast<void *>(offsetof(Self, u)));
    glEnableVertexAttribArray(1);
    // nx, ny, nz normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Self),
      reinterpret_cast<void *>(offsetof(Self, nx)));
    glEnableVertexAttribArray(2);
  }

  constexpr Vertex(float x = {}, float y = {}, float z = {}, float u = {},
                   float v = {}, float nx = {}, float ny = {}, float nz = {})
      : x(x), y(y), z(z), u(u), v(v), nx(nx), ny(ny), nz(nz) {}

  // —————————————————————————— //

  static_assert(Is_Vertex<Self>);
	};

}
