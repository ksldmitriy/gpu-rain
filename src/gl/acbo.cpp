#include "acbo.hpp"

#include <glad/gl.h>
#include <stdexcept>
#include <utility>

namespace gl {

ACBO::ACBO() {
}

ACBO::ACBO(ACBO &&acbo) {
  *this = std::move(acbo);
}

ACBO &ACBO::operator=(ACBO &&acbo) {
  if (IsCreated()) {
    throw std::runtime_error("trying to move acbo over exising one");
  }

  m_handle = std::exchange(acbo.m_handle, 0);
  m_count = std::exchange(acbo.m_count, 0);

  return *this;
}

ACBO::~ACBO() {
  if (!IsCreated()) {
    return;
  }

  Destroy();
}

void ACBO::Create(size_t count, GLenum mode, GLuint *data) {
  if (IsCreated()) {
    throw std::runtime_error("trying to crea acbo over existing one");
  }

  m_count = count;

  glGenBuffers(1, &m_handle);
  glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_handle);

  glBufferData(GL_ATOMIC_COUNTER_BUFFER, count * sizeof(GLuint), data, mode);
}

void ACBO::Destroy() {
  if (!IsCreated()) {
    throw std::runtime_error("trying to delete non existing acbo");
  }

  glDeleteBuffers(1, &m_handle);
  m_count = 0;
}

void ACBO::Bind(size_t index) {
  glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, index, m_handle);
}

size_t ACBO::GetCount() {
  return m_count;
}

bool ACBO::IsCreated() {
  return m_handle != 0;
}

}; // namespace gl
