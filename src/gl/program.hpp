#pragma once
#include <string>

namespace gl {

typedef unsigned int GLuint, GLenum;

class Shader;

class Program {
public:
  Program();
  Program(Program &) = delete;
  Program &operator=(Program &) = delete;
  Program(Program &&program);
  Program &operator=(Program &&program);
  ~Program();

  void Create();
  void AttachShader(Shader &shader);
  void Link();
  void Delete();

  bool IsSuccessfullyLinked();
  const std::string &GetLinkError();

  void Use() const;

private:
  void LinkErrorHandling();

private:
  GLuint m_handle = 0;
  std::string m_link_error_message;
};

} // namespace gl
