#pragma once

#include "toolkit/intdef.h"
namespace pg::err {

	struct GLFramebufferIncomplete {
		GLenum gl_error;
		std::string str() const {
			return std::string("Framebuffer marked as incomplete with OpenGL Error Code: ") + std::to_string(gl_error);
		}
	};

	struct GLFramebufferInvalidResize {
		GLenum gl_error;
		std::string str() const {
			return std::string("Framebuffer resizing failed with OpenGL Error: ") + std::to_string(gl_error);
		}
	};

}
