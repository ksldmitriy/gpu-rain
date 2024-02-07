#pragma once
#include <string>

namespace gl {

typedef unsigned int GLuint, GLenum;
typedef int GLint;

class Shader {
public:
  Shader();
  Shader(Shader &) = delete;
  Shader &operator=(Shader &) = delete;
  Shader(Shader &&);
  Shader &operator=(Shader &&);
  ~Shader();

  void Create(const char source[], GLint source_len, GLenum type);
  void Delete();

  bool IsSuccessfullyCompiled();
  const std::string &GetCompileError();

  GLuint GetHandle();

private:
  void CompileErrorHandling();

private:
  GLuint m_handle = 0;
  std::string m_compile_error_message;
};

} // namespace gl
