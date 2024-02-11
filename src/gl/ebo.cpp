#include "ebo.hpp"

#include <glad/gl.h>
#include <stdexcept>

namespace gl {

EBO::EBO() {
}

EBO::EBO(EBO &&ebo) {
  *this = std::move(ebo);
}

EBO &EBO::operator=(EBO &&ebo) {
  if (m_handle) {
    throw std::runtime_error("trying to move ebo to existing one");
  }

  m_handle = ebo.m_handle;
  ebo.m_handle = 0;

  return *this;
}

EBO::~EBO() {
  if (!m_handle) {
    return;
  }

  Destroy();
}

void EBO::Create(size_t count, GLenum mode, const GLuint *data) {
  if (m_handle) {
    throw std::runtime_error("trying to create ebo over existing one");
  }

  glGenBuffers(1, &m_handle);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, mode);
}

void EBO::Destroy() {
  if (!m_handle) {
    throw std::runtime_error("trying to destroy non existing ebo");
  }

  glDeleteBuffers(1, &m_handle);
}

void EBO::Bind() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
}

} // namespace gl
