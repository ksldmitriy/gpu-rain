#pragma once
#include <glm/glm.hpp>
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

  void SetUniformInt(const char *name, int value);
  void SetUniformFloat(const char *name, float value);
  void SetUniformFVec3(const char *name, glm::fvec3 value);
  void SetUniformMat4f(const char *name, glm::mat<4, 4, float> value);

  static Program CreateProgram(const char vert_source[], size_t vert_source_len,
                               const char frag_source[],
                               size_t frag_source_len);

private:
  void LinkErrorHandling();

private:
  GLuint m_handle = 0;
  std::string m_link_error_message;
};

} // namespace gl
