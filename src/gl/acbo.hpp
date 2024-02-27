#pragma once
#include <stddef.h>

namespace gl {

typedef unsigned int GLuint, GLenum;

// ACBO stands for Atomic Counter Buffer Object

class ACBO {
public:
  ACBO();
  ACBO(ACBO &) = delete;
  ACBO &operator=(ACBO &) = delete;
  ACBO(ACBO &&acbo);
  ACBO &operator=(ACBO &&acbo);
  ~ACBO();

  void Create(size_t count, GLenum mode, GLuint *data = nullptr);
  void Destroy();

  void Bind(size_t index);
  size_t GetCount();
  bool IsCreated();

private:
private:
  GLuint m_handle = 0;
  size_t m_count = 0;
};

} // namespace gl
