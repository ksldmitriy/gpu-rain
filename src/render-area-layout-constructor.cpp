#include "render-area-layout-constructor.hpp"

#include "render-area-layout.hpp"
#include "x-root-window.hpp"

#include <glad/gl.h>

void RenderAreaLayoutConstructor::ConstructLayout(
    const std::vector<MonitorDimensions> &monitors, glm::uvec2 framebuffer_size,
    glm::uvec2 screen_size, RenderAreaLayout &render_layout) {
  m_screen_size = screen_size;
  m_framebuffer_size = framebuffer_size;

  std::vector<Vertex> vertices;

  for (const MonitorDimensions &monitor : monitors) {
    MonitorVertices monitor_vertices = ConstructLayoutForMonitor(monitor);
    vertices.insert(vertices.end(), monitor_vertices.begin(),
                    monitor_vertices.end());
  }

  CreateVbo(vertices, render_layout.vbo);
  CreateVao(render_layout.vbo, render_layout.vao);
  CreateEbo(monitors.size(), render_layout.ebo);

  render_layout.index_count = monitors.size() * 6;
}

void RenderAreaLayoutConstructor::CreateVbo(const std::vector<Vertex> &vertices,
                                            gl::VBO &vbo) {
  vbo.Create(vertices.size() * sizeof(Vertex), GL_STATIC_DRAW, vertices.data());
}

void RenderAreaLayoutConstructor::CreateVao(const gl::VBO &vbo, gl::VAO &vao) {
  vao.Create(s_vertex_fields, sizeof(Vertex));
  vbo.Bind();
  vao.PointAttribs();
}

void RenderAreaLayoutConstructor::CreateEbo(size_t areas_count, gl::EBO &ebo) {
  std::vector<GLuint> indices;

  for (int i = 0; i < areas_count; i++) {
    size_t vertex_offset = i * 4;

    indices.push_back(vertex_offset + 0);
    indices.push_back(vertex_offset + 1);
    indices.push_back(vertex_offset + 2);
    indices.push_back(vertex_offset + 0);
    indices.push_back(vertex_offset + 2);
    indices.push_back(vertex_offset + 3);
  }

  ebo.Create(indices.size(), GL_STATIC_DRAW, indices.data());
}

RenderAreaLayoutConstructor::MonitorVertices
RenderAreaLayoutConstructor::ConstructLayoutForMonitor(
    const MonitorDimensions &monitor) {
  MonitorVertices vertices;

  CalculateAreaPosition(monitor, vertices);
  CalculateAreaUv(monitor, vertices);

  return vertices;
}

void RenderAreaLayoutConstructor::CalculateAreaPosition(
    const MonitorDimensions &monitor, MonitorVertices &vertices) {
  // clang-format off
  glm::uvec2 pixel_positions[4];
  pixel_positions[0] = monitor.position + glm::ivec2(0, 0);
  pixel_positions[1] = monitor.position + glm::ivec2(monitor.size.x, 0);
  pixel_positions[2] = monitor.position + glm::ivec2(monitor.size.x, monitor.size.y);
  pixel_positions[3] = monitor.position + glm::ivec2(0, monitor.size.y);
  // clang-format on

  vertices[0].pos = ScreenToGlPos(pixel_positions[0]);
  vertices[1].pos = ScreenToGlPos(pixel_positions[1]);
  vertices[2].pos = ScreenToGlPos(pixel_positions[2]);
  vertices[3].pos = ScreenToGlPos(pixel_positions[3]);
}

void RenderAreaLayoutConstructor::CalculateAreaUv(
    const MonitorDimensions &monitor, MonitorVertices &vertices) {
  vertices[0].uv = glm::fvec2(0, 0);
  vertices[1].uv = glm::fvec2(1, 0);
  vertices[2].uv = glm::fvec2(1, 1);
  vertices[3].uv = glm::fvec2(0, 1);
}

glm::fvec2 RenderAreaLayoutConstructor::ScreenToGlPos(glm::ivec2 pos) {
  glm::fvec2 gl_position = glm::fvec2(pos) / (m_screen_size / 2.f);

  gl_position -= glm::fvec2(1, 1);

  return gl_position;
}

const std::vector<gl::VAO::VertexField>
    RenderAreaLayoutConstructor::s_vertex_fields = {
        {2, GL_FLOAT, offsetof(Vertex, pos)},
        {2, GL_FLOAT, offsetof(Vertex, uv)},
};
