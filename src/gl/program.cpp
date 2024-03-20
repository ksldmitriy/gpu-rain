#include "program.hpp"

#include "shader.hpp"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
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

  m_link_error_message = std::move(program.m_link_error_message);

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
  if (!m_handle) {
    throw std::runtime_error("trying attach shader to non existing program");
  }

  glAttachShader(m_handle, shader.GetHandle());
}

void Program::Link() {
  if (!m_handle) {
    throw std::runtime_error("trying link non existing program");
  }

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
  if (!m_handle) {
    throw std::runtime_error("trying to use non existing program");
  }

  glUseProgram(m_handle);
}

void Program::SetUniformInt(const char *name, int value) {
  GLint location = glGetUniformLocation(m_handle, name);

  glUseProgram(m_handle);
  glUniform1i(location, value);
}

void Program::SetUniformFloat(const char *name, float value) {
  GLint location = glGetUniformLocation(m_handle, name);

  glUseProgram(m_handle);
  glUniform1f(location, value);
}

void Program::SetUniformFVec3(const char *name, glm::fvec3 value) {
  GLint location = glGetUniformLocation(m_handle, name);

  glUseProgram(m_handle);
  glUniform3fv(location, 1, glm::value_ptr(value));
}

void Program::SetUniformFVec2(const char *name, glm::fvec2 value) {
  GLint location = glGetUniformLocation(m_handle, name);

  glUseProgram(m_handle);
  glUniform2fv(location, 1, glm::value_ptr(value));
}

void Program::SetUniformMat4f(const char *name, glm::mat<4, 4, float> value) {
  GLint location = glGetUniformLocation(m_handle, name);

  glUseProgram(m_handle);
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

Program Program::CreateProgram(const char vert_source[], size_t vert_source_len,
                               const char frag_source[],
                               size_t frag_source_len) {
  bool success;

  Shader vertex_shader;
  vertex_shader.Create(vert_source, vert_source_len, GL_VERTEX_SHADER);
  success = vertex_shader.IsSuccessfullyCompiled();
  if (!success) {
    throw std::runtime_error(vertex_shader.GetCompileError());
  }

  Shader fragment_shader;
  fragment_shader.Create(frag_source, frag_source_len, GL_FRAGMENT_SHADER);
  success = fragment_shader.IsSuccessfullyCompiled();
  if (!success) {
    throw std::runtime_error(fragment_shader.GetCompileError());
  }

  Program program;
  program.Create();
  program.AttachShader(vertex_shader);
  program.AttachShader(fragment_shader);
  program.Link();
  success = program.IsSuccessfullyLinked();
  if (!success) {
    throw std::runtime_error(program.GetLinkError());
  }

  vertex_shader.Delete();
  fragment_shader.Delete();

  return std::move(program);
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
