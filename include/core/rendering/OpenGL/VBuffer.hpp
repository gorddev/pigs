#pragma once

#include <ostream>

#include "PVertex.hpp"
#include "toolkit/apidef.h"
#include <core/errors/unwrap.hpp>
#include <span>
#include <glm/mat4x4.hpp>

// Created by Gordie Novak on 2/17/26.

// vertex buffers hold all the information needed
// to render an object that has already been added
// to open gl.

typedef uint32_t GLuint; //< typedef so we can use w/o includes.

namespace pg {

    /// A vertex buffer is an internal engine object
    /// that stores internal vertex and rendering information
    /// for openGL rendering.
    template<typename Vertex_t = Vertex>
        requires(Is_Vertex<Vertex_t>)
    struct VBuffer {
    private:
        GLuint vao;     ///< Vertex Array handle
        GLuint vbo;     ///< Vertex Buffer handle
        GLuint mbo;     ///< Matrix buffer handle
        GLuint cbo;     ///< Color buffer handle.
        uint32_t vertexCount;  ///< Number of indices

    public:

    /* ******************* Constructors *********************** */

        VBuffer() = default;
        ~VBuffer();

        VBuffer(const VBuffer& other)               = delete;
        VBuffer& operator=(const VBuffer& other)    = delete;

        VBuffer(VBuffer&& other) noexcept;
        VBuffer& operator=(VBuffer&& other) noexcept;

    /* **************************************************************************** */

        /** Makes a VertexBuffer with OpenGL commands
         * @param vertices An array of vertices to upload
         * @param numVertices The number of vertices in the array
         * @return A properly initialized @code VertexBuffer@endcode object.
         */
        [[nodiscard]] static VBuffer make(const Vertex_t vertices[], size_t numVertices);

    /* **************************************************************************** */

        /** Generates a matrix VBO for a specific vertex buffer object.
         * @param buffer The VertexBuffer you want to attach this upload to
         * @param shaderLoc The location in the shader you want to upload the data to. Must be divisible by 4.
         * @return An optional GLuint: exists if creation was successful
         * @warning shaderLoc must be divisible by four. */
        [[nodiscard]] Err genMatrixBuffer(size_t shaderLoc);
        void updateMatrixBuffer(std::span<const glm::mat4>);

        [[nodiscard]] Err genColorVBO(size_t shaderLoc);
        void updateColorBuffer(std::span<const vec4> colors);

        VBuffer& glBind();
        VBuffer& glDraw();
        VBuffer& glUnbind();
        VBuffer& glBindVBO();

    };

    VBuffer() -> VBuffer<>;

    #include "VBuffer.inl"
}