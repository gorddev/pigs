#pragma once

#include "Is_Vertex.hpp"
#include <array>
#include <toolkit/types/vec.hpp>
#include <cstdint>

namespace pg {

	/** (Texture Vertex): Barebones vertex with two position attributes (x,y)
 * and nothing else, with (u,v) always being  Intended to be used with fragment shaders for various
 * purposes. */
	struct TextureVertex {
	  using Self = TextureVertex;
	  static constexpr size_t num_attributes = 2;

	  float x, y; //< position data
	  float u, v; //< uv data

	  TextureVertex(float x, float y) : x(x), y(y), u(x), v(y) {}
	  TextureVertex(float x, float y, float u, float v) : x(x), y(y), u(u), v(v) {}

	  static void setup_attributes() {
	    // setting up (x,y)
	    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Self),
	      reinterpret_cast<void *>(offsetof(Self, x)));
	    glEnableVertexAttribArray(0);
	    // setting up (u,v)
	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Self),
	                          reinterpret_cast<void *>(offsetof(Self, u)));
	    glEnableVertexAttribArray(1);
	  }

		static std::array<TextureVertex, 6> full_quad(vec2 top_left, vec2 bottom_right) {
			return {
				TextureVertex{top_left.x, top_left.y, 0.0, 0.0},
				{bottom_right.x, top_left.y, 1.0, 0.0},
				{top_left.x, bottom_right.y, 0.0, 1.0},
				{top_left.x, bottom_right.y, 0.0, 1.0},
				{bottom_right.x, bottom_right.y, 1.0, 1.0},
				{bottom_right.x, top_left.y, 1.0, 0.0},
			};
		}

	  // —————————————————————————— //
	  static_assert(Is_Vertex<Self>);
	};

	using TVertex = TextureVertex;
}
