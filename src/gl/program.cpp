#include "program.hpp"

#include "shader.hpp"

#include <glad/gl.h>
#include <stdexcept>

namespace gl {

Program::Program() {
}

Program::Program(Program &&program) {
  *this = std::move(program);
}

Program &Program::operator=(Program &&program) {
  if (m_handle) {
    throw std::runtime_error("trying to move program to existing one");
  }

  m_handle = program.m_handle;
  program.m_handle = 0;

  return *this;
}

Program::~Program() {
  if (!m_handle) {
    return;
  }

  Delete();
}

void Program::Create() {
  if (m_handle) {
    throw std::runtime_error("trying to create Program over exising one");
  }

  m_handle = glCreateProgram();
}

void Program::AttachShader(Shader &shader) {
  glAttachShader(m_handle, shader.GetHandle());
}

void Program::Link() {
  glLinkProgram(m_handle);

  GLint compile_status;
  glGetProgramiv(m_handle, GL_LINK_STATUS, &compile_status);
  if (!compile_status) {
    LinkErrorHandling();
  }
}

void Program::Delete() {
  if (!m_handle) {
    throw std::runtime_error("trying to delete non existing program");
  }

  glDeleteProgram(m_handle);
  m_handle = 0;
}

bool Program::IsSuccessfullyLinked() {
  return m_link_error_message.empty();
}

const std::string &Program::GetLinkError() {
  return m_link_error_message;
}

void Program::Use() const {
  glUseProgram(m_handle);
}

void Program::LinkErrorHandling() {
  GLint info_len;
  glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &info_len);

  char *info_buffer = new char[info_len];
  glGetProgramInfoLog(m_handle, info_len, nullptr, info_buffer);

  m_link_error_message = std::string("shader compile error: ") + info_buffer;

  delete[] info_buffer;
}

} // namespace gl
