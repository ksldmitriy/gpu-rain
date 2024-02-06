#include "vbo.hpp"

#include "glad/gl.h"
#include <stdexcept>

namespace gl {

VBO::VBO() {
}

VBO::VBO(VBO &&vbo) {
  *this = std::move(vbo);
}

VBO &VBO::operator=(VBO &&vbo) {
  if (m_handle) {
    throw std::runtime_error("trying to move vbo to existing one");
  }

  m_handle = vbo.m_handle;
  m_size = vbo.m_size;

  vbo.m_handle = 0;
  vbo.m_size = 0;

  return *this;
}

VBO::~VBO() {
  if (!m_handle) {
    return;
  }

  Free();
}

void VBO::Create(size_t size, GLenum mode, const void *data) {
  if (m_handle) {
    throw std::runtime_error("trying to create VBO over existing one");
  }

  if (!size) {
    throw std::runtime_error("trying to create VBO with size 0");
  }

  m_size = 0;

  glGenBuffers(1, &m_handle);
  glBindBuffer(GL_ARRAY_BUFFER, m_handle);

  glBufferData(m_handle, size, data, mode);
}

void VBO::Free() {
  if (!m_handle) {
    throw std::runtime_error("trying to free empty VBO");
  }

  m_size = 0;

  glDeleteBuffers(1, &m_handle);
}

void VBO::Bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, m_handle);
}

size_t VBO::GetSize() const {
  return m_size;
}

} // namespace gl
