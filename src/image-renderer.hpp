#pragma once
#include "gl/ebo.hpp"
#include "gl/program.hpp"
#include "gl/texture.hpp"
#include "gl/vao.hpp"
#include "gl/vbo.hpp"

#include <filesystem>

namespace fs = std::filesystem;

typedef unsigned int GLuint;

class ImageRenderer {
private:
  struct Vertex {
    glm::fvec3 pos;
    glm::fvec2 uv;
  };

public:
  ImageRenderer();
  ImageRenderer(ImageRenderer &) = delete;
  ImageRenderer &operator=(ImageRenderer &) = delete;
  ImageRenderer(ImageRenderer &&image_renderer);
  ImageRenderer &operator=(ImageRenderer &&image_renderer);
  ~ImageRenderer();

  void Create(fs::path image_path);
  void Destroy();

  void Draw();
  
private:
  void LoadImage(fs::path image_path);
  void CreateTexture(const unsigned char *image_data, glm::uvec2 size);

private:
  gl::VBO m_vbo;
  gl::VAO m_vao;
  gl::EBO m_ebo;

  gl::Texture m_texture;

  bool m_is_created = false;
  
  static const std::array<Vertex, 4> s_vertices;
  static const std::vector<gl::VAO::VertexField> s_vertex_fields;
  static const std::array<GLuint, 6> s_indices;
};
