#include "shader.hpp"

#include <glad/gl.h>
#include <stdexcept>

#include <iostream>

namespace gl {

Shader::Shader() {
}

Shader::Shader(Shader &&shader) {
  *this = std::move(shader);
}

Shader &Shader::operator=(Shader &&shader) {
  if (m_handle) {
    throw std::runtime_error("trying to move shader to existing one");
  }

  m_handle = shader.m_handle;
  shader.m_handle = 0;

  return *this;
}

Shader::~Shader() {
  if (!m_handle) {
    return;
  }

  Delete();
}

void Shader::Create(const char source[], GLint source_len, GLenum type) {
  if (m_handle) {
    throw std::runtime_error("create shader over existing one");
  }

  m_compile_error_message.clear();

  m_handle = glCreateShader(type);
  glShaderSource(m_handle, 1, &source, &source_len);
  glCompileShader(m_handle);

  GLint compile_status;
  glGetShaderiv(m_handle, GL_COMPILE_STATUS, &compile_status);
  if (!compile_status) {
    CompileErrorHandling();
  }
}

void Shader::Delete() {
  if (!m_handle) {
    throw std::runtime_error("trying to delete non existing shader");
  }

  glDeleteShader(m_handle);
  m_handle = 0;
}

bool Shader::IsSuccessfullyCompiled() {
  return m_compile_error_message.empty();
}

const std::string &Shader::GetCompileError() {
  return m_compile_error_message;
}

GLuint Shader::GetHandle() {
  return m_handle;
}

void Shader::CompileErrorHandling() {
  GLint info_len;
  glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &info_len);

  char *info_buffer = new char[info_len];
  glGetShaderInfoLog(m_handle, info_len, nullptr, info_buffer);

  m_compile_error_message = std::string("shader compile error: ") + info_buffer;

  delete[] info_buffer;
}

} // namespace gl
