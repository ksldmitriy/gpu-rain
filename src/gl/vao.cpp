#include "vao.hpp"

#include "glad/gl.h"
#include <stdexcept>

namespace gl {

VertexArray::VertexArray() {
}

VertexArray::~VertexArray() {
  if (!m_handle) {
    return;
  }

  Destroy();
}

void VertexArray::Create(const std::vector<VertexField> &vertex_fields,
                         size_t stride) {
  if (m_handle) {
    throw std::runtime_error("trying to create VAO over existing one");
  }

  glGenVertexArrays(1, &m_handle);
  glBindVertexArray(m_handle);

  m_vertex_fields = vertex_fields;
  m_stride = stride;
}

void VertexArray::Destroy() {
  if (!m_handle) {
    throw std::runtime_error("trying to destroy empty VAO");
  }

  glDeleteBuffers(1, &m_handle);
  m_handle = 0;

  m_stride = 0;
  m_vertex_fields.clear();
}

void VertexArray::Bind() const {
  glBindVertexArray(m_handle);
}

void VertexArray::PointAttribs() {
  glBindVertexArray(m_handle);

  for (int i = 0; i < m_vertex_fields.size(); i++) {
    VertexField &field = m_vertex_fields[i];

    glVertexAttribPointer(i, field.count, field.type, GL_FALSE, m_stride,
                          (void *)field.offset);
    glEnableVertexAttribArray(i);
  }
}

} // namespace gl
