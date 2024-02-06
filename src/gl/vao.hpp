#pragma once
#include <vector>

namespace gl {

typedef unsigned int GLuint, GLenum;

class VertexArray {
public:
  struct VertexField {
    int count;
    unsigned int type;
    size_t offset;
  };

public:
  VertexArray();
  VertexArray(VertexArray &) = delete;
  VertexArray &operator=(VertexArray &) = delete;
  VertexArray(VertexArray &&vao);
  VertexArray &operator=(VertexArray &&vao);
  ~VertexArray();

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
