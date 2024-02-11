#pragma once
#include "gl/vao.hpp"

#include <array>
#include <glm/glm.hpp>
#include <vector>

struct RenderAreaLayout;
struct MonitorDimensions;

namespace gl {
class VBO;
class VAO;
class EBO;
} // namespace gl

class RenderAreaLayoutConstructor {
private:
  struct Vertex {
    glm::fvec2 pos;
    glm::fvec2 uv;
  };

  typedef std::array<Vertex, 4> MonitorVertices;

public:
  void ConstructLayout(const std::vector<MonitorDimensions> &monitors,
                       glm::uvec2 framebuffer_size, glm::uvec2 screen_size,
                       RenderAreaLayout &render_layout);

private:
  void CreateVbo(const std::vector<Vertex> &vertices, gl::VBO &vbo);
  void CreateVao(const gl::VBO &vbo, gl::VAO &vao);
  void CreateEbo(size_t areas_count, gl::EBO &ebo);

  MonitorVertices ConstructLayoutForMonitor(const MonitorDimensions &monitor);

  void CalculateAreaPosition(const MonitorDimensions &monitor,
                             MonitorVertices &vertices);
  void CalculateAreaUv(const MonitorDimensions &monitor,
                       MonitorVertices &vertices);
  glm::fvec2 ScreenToGlPos(glm::ivec2 pos);

private:
  glm::fvec2 m_screen_size;
  glm::uvec2 m_framebuffer_size;

  static const std::vector<gl::VAO::VertexField> s_vertex_fields;
};
