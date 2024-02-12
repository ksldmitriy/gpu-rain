#pragma once
#include <stddef.h>
#include <stdint.h>

namespace gl {

typedef unsigned int GLuint, GLenum;

class SSBO {
public:
  SSBO();
  SSBO(SSBO &) = delete;
  SSBO &operator=(SSBO &) = delete;
  SSBO(SSBO &&ssbo);
  SSBO &operator=(SSBO &&ssbo);
  ~SSBO();

  void Create(size_t size, GLenum usage, void *data = nullptr);
  void Destroy();

  void Bind(GLuint index);
  size_t GetSize() const;

  void* Map(GLenum access);
  void Unmap();
  
private:
private:
  GLuint m_handle = 0;
  size_t m_size = 0;
};

} // namespace gl
