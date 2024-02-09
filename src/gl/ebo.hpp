#pragma once
#include <stddef.h>

namespace gl {

typedef unsigned int GLuint, GLenum;

class EBO {
public:
  EBO();
  EBO(EBO &) = delete;
  EBO &operator=(EBO &) = delete;
  EBO(EBO &&ebo);
  EBO &operator=(EBO &&ebo);
  ~EBO();

  void Create(size_t count, GLenum mode, GLuint *data = nullptr);
  void Destroy();

  void Bind();
  
private:
private:
  GLuint m_handle = 0;
};

} // namespace gl
