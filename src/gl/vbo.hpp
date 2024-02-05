#pragma once
#include <stddef.h>
#include <stdint.h>

namespace gl {

typedef unsigned int GLuint, GLenum;

class VBO {
public:
  VBO();
  VBO(VBO &) = delete;
  VBO &operator=(VBO &) = delete;
  ~VBO();

  void Create(size_t size, GLenum mode, const void *data = nullptr);
  void Free();

  void Bind() const;
  
  size_t GetSize() const;

private:
private:
  GLuint m_handle = 0;
  int m_size = 0;
};

} // namespace gl
