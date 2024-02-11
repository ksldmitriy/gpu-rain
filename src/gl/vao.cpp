#include "vao.hpp"

#include "glad/gl.h"
#include <stdexcept>

namespace gl {

VAO::VAO() {
}

VAO::VAO(VAO &&vao) {
  *this = std::move(vao);
}

VAO &VAO::operator=(VAO &&vao) {
  if (m_handle) {
    throw std::runtime_error("trying to move vao to existing one");
  }

  m_handle = vao.m_handle;
  m_stride = vao.m_stride;
  m_vertex_fields = std::move(vao.m_vertex_fields);

  vao.m_handle = 0;
  vao.m_stride = 0;

  return *this;
}

VAO::~VAO() {
  if (!m_handle) {
    return;
  }

  Destroy();
}

void VAO::Create(const std::vector<VertexField> &vertex_fields,
                         size_t stride) {
  if (m_handle) {
    throw std::runtime_error("trying to create VAO over existing one");
  }

  glGenVertexArrays(1, &m_handle);
  glBindVertexArray(m_handle);

  m_vertex_fields = vertex_fields;
  m_stride = stride;
}

void VAO::Destroy() {
  if (!m_handle) {
    throw std::runtime_error("trying to destroy empty VAO");
  }

  glDeleteBuffers(1, &m_handle);
  m_handle = 0;

  m_stride = 0;
  m_vertex_fields.clear();
}

void VAO::Bind() const {
  glBindVertexArray(m_handle);
}

void VAO::PointAttribs() {
  glBindVertexArray(m_handle);

  for (int i = 0; i < m_vertex_fields.size(); i++) {
    VertexField &field = m_vertex_fields[i];

    glVertexAttribPointer(i, field.count, field.type, GL_FALSE, m_stride,
                          (void *)field.offset);
    glEnableVertexAttribArray(i);
  }
}

} // namespace gl
