#pragma once
#include <vector>

namespace gl {

typedef unsigned int GLuint, GLenum;

class VAO {
public:
  struct VertexField {
    int count;
    unsigned int type;
    size_t offset;
  };

public:
  VAO();
  VAO(VAO &) = delete;
  VAO &operator=(VAO &) = delete;
  VAO(VAO &&vao);
  VAO &operator=(VAO &&vao);
  ~VAO();

  void Create(const std::vector<VertexField> &vertex_fields, size_t stride);
  void Destroy();

  void Bind() const;
  void PointAttribs();

private:
  GLuint m_handle = 0;
  std::vector<VertexField> m_vertex_fields;
  size_t m_stride;
};

} // namespace gl
