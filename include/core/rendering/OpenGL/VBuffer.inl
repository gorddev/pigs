#pragma once
// ************************************************ //

#include "errors/Error.hpp"
#include "errors/error-structs/OpenGL/GLVertexBufferErrors.hpp"
#include <optional>
template <typename V>
  requires(Is_Vertex<V>)
VBuffer<V>::VBuffer(VBuffer &&other) noexcept
    : vao(other.vao), vbo(other.vbo), mbo(other.mbo), cbo(other.cbo),
      vertexCount(other.vertexCount) {
  other.vao = 0;
  other.vbo = 0;
  other.mbo = 0;
  other.cbo = 0;
  other.vertexCount = 0;
}

// ************************************************ //

template <typename V>
  requires(Is_Vertex<V>)
VBuffer<V> &VBuffer<V>::operator=(VBuffer &&other) noexcept {
  std::swap(vao, other.vao);
  std::swap(vbo, other.vbo);
  std::swap(mbo, other.mbo);
  std::swap(cbo, other.cbo);
  std::swap(vertexCount, other.vertexCount);
  return *this;
}

// ************************************************ //

template <typename V>
  requires(Is_Vertex<V>)
VBuffer<V> VBuffer<V>::make(const V vertices[], const size_t numVertices) {
  VBuffer buffer{};

  glGenVertexArrays(1, &buffer.vao);
  glBindVertexArray(buffer.vao);

  glGenBuffers(1, &buffer.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
  glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(V), vertices,
               GL_STATIC_DRAW);

  // allow the vertex type to setup its own attributes
  glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
  V::setup_attributes();

  // unbind our vertex array.
  glBindVertexArray(0);

  buffer.vertexCount = numVertices;
  buffer.mbo = 0;
  buffer.cbo = 0;

  return buffer;
}

// ************************************************ //

template <typename V>
  requires(Is_Vertex<V>)
optional_err<err::GLMatrixBufferInvalidShaderLoc>
VBuffer<V>::genMatrixBuffer(size_t shaderLoc) {
  if (shaderLoc % 4 != 0 || shaderLoc < V::num_attributes) {
    return PG_ErrNew(err::GLMatrixBufferInvalidShaderLoc,
	    .provided_value = shaderLoc,
	    .minimum_attributes = V::num_attributes);
  }
  glBindVertexArray(vao);
  glGenBuffers(1, &mbo); //< generate buffer into our new vbo
  glBindBuffer(GL_ARRAY_BUFFER, mbo);
  for (size_t i = 0; i < 4; i++) {
    glEnableVertexAttribArray(i + shaderLoc);

    glVertexAttribPointer(shaderLoc + i, 4, GL_FLOAT, GL_FALSE,
      sizeof(float) * 16, //< size of 4x4 float matrix
      reinterpret_cast<void *>(sizeof(float) * 4 * i)); //< offset we render to

    glVertexAttribDivisor(shaderLoc + i, 1);
  }
  return std::nullopt;
}

template <typename Vertex_t>
  requires(Is_Vertex<Vertex_t>)
void VBuffer<Vertex_t>::updateMatrixBuffer(std::span<const glm::mat4> mats) {
  glBindBuffer(GL_ARRAY_BUFFER, mbo);
  // TODO: Try keeping track of current count of models so we can potentially
  // use glBufferSubData
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(sizeof(glm::mat4) * mats.size()),
               mats.data(), GL_DYNAMIC_DRAW);
}

// ************************************************ //

template <typename V>
  requires(Is_Vertex<V>)
optional_err<err::GLColorBufferInvalidShaderLoc>
VBuffer<V>::genColorVBO(size_t shaderLoc) {
  if (shaderLoc <= V::num_attributes) {
    return PG_ErrNew(err::GLColorBufferInvalidShaderLoc,
                     .provided_value = shaderLoc,
                     .num_attributes = V::num_attributes);
  }
  glBindVertexArray(vao);
  glGenBuffers(1, &cbo);
  glBindBuffer(GL_ARRAY_BUFFER, cbo);
  glVertexAttribPointer(shaderLoc, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
  glEnableVertexAttribArray(shaderLoc);
  return std::nullopt;
}

template <typename V>
  requires(Is_Vertex<V>)
void VBuffer<V>::updateColorBuffer(std::span<const vec4> colors) {
  glBindBuffer(GL_ARRAY_BUFFER, cbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * colors.size(), colors.data(),
               GL_DYNAMIC_DRAW);
}

template <typename V>
  requires(Is_Vertex<V>)
VBuffer<V>::~VBuffer() {
  glBindVertexArray(vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &mbo);
  glDeleteBuffers(1, &cbo);

  glDeleteVertexArrays(1, &vao);
  glBindVertexArray(0);
}

template <typename V>
  requires(Is_Vertex<V>)
VBuffer<V> &VBuffer<V>::glBind() {
  glBindVertexArray(this->vao);
  return *this;
}

template <typename V>
  requires(Is_Vertex<V>)
VBuffer<V> &VBuffer<V>::glBindVBO() {
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  return *this;
}

template <typename Vertex_t>
  requires(Is_Vertex<Vertex_t>)
VBuffer<Vertex_t> &VBuffer<Vertex_t>::glDraw() {
  glDrawArrays(GL_TRIANGLES, 0, vertexCount);
  return *this;
}

template <typename Vertex_t>
  requires(Is_Vertex<Vertex_t>)
VBuffer<Vertex_t> &VBuffer<Vertex_t>::glUnbind() {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return *this;
}
