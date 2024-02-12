#include "ssbo.hpp"

#include <glad/gl.h>
#include <stdexcept>

namespace gl {

SSBO::SSBO() {
}

SSBO::SSBO(SSBO &&ssbo) {
  *this = std::move(ssbo);
}

SSBO &SSBO::operator=(SSBO &&ssbo) {
  if (m_handle) {
    throw std::runtime_error("trying to move ssbo to existing one");
  }

  m_handle = ssbo.m_handle;
  m_size = ssbo.m_size;

  ssbo.m_handle = 0;
  ssbo.m_size = 0;

  return *this;
}

SSBO::~SSBO() {
  if (!m_handle) {
    return;
  }

  Destroy();
}

void SSBO::Create(size_t size, GLenum usage, void *data) {
  if (m_handle) {
    throw std::runtime_error("trying to create ssbo over existing one");
  }

  glGenBuffers(1, &m_handle);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_handle);
  glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
}

void SSBO::Destroy() {
  if (!m_handle) {
    throw std::runtime_error("trying to delete non existing ssbo");
  }

  glDeleteBuffers(1, &m_handle);
}

void SSBO::Bind(GLuint index) {
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_handle);
}

size_t SSBO::GetSize() const {
  return m_size;
}

void *SSBO::Map(GLenum access) {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_handle);
  return glMapBuffer(GL_SHADER_STORAGE_BUFFER, access);
}

void SSBO::Unmap() {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_handle);
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

} // namespace gl
