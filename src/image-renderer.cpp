#include "image-renderer.hpp"

#include "programs-library.hpp"

#include <array>
#include <glad/gl.h>
#include <stb/stb_image.h>

ImageRenderer::ImageRenderer() {
}

ImageRenderer::ImageRenderer(ImageRenderer &&image_renderer) {
  *this = std::move(image_renderer);
}

ImageRenderer &ImageRenderer::operator=(ImageRenderer &&image_renderer) {
  if (m_is_created) {
    throw std::runtime_error("trying to move image renderer to existing one");
  }

  m_vbo = std::move(image_renderer.m_vbo);
  m_vao = std::move(image_renderer.m_vao);
  m_ebo = std::move(image_renderer.m_ebo);
  m_texture = std::move(image_renderer.m_texture);

  image_renderer.m_is_created = false;
  m_is_created = true;

  return *this;
}

ImageRenderer::~ImageRenderer() {
  if (!m_is_created) {
    return;
  }

  Destroy();
}

void ImageRenderer::Create(fs::path image_path) {
  if (m_is_created) {
    throw std::runtime_error(
        "trying to create ImageRenderer over existing one");
  }

  LoadImage(image_path);

  m_vbo.Create(s_vertices.size() * sizeof(Vertex), GL_STATIC_DRAW,
               s_vertices.data());

  m_vao.Create(s_vertex_fields, sizeof(Vertex));
  m_vbo.Bind();
  m_vao.PointAttribs();

  m_ebo.Create(6, GL_STATIC_DRAW, s_indices.data());

  m_is_created = true;
}

void ImageRenderer::Destroy() {
  if (!m_is_created) {
    throw std::runtime_error("trying to destroy non existing image renderer");
  }

  m_texture.Delete();
  m_vbo.Free();
  m_vao.Destroy();
  m_ebo.Destroy();

  m_is_created = false;
}

void ImageRenderer::Draw() {
  if (!m_is_created) {
    throw std::runtime_error("trying to draw non existing image renderer");
  }

  ProgramsLibrary::GetTextureProgram().Use();

  m_texture.Bind();
  m_vao.Bind();
  m_ebo.Bind();

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void ImageRenderer::LoadImage(fs::path image_path) {
  int x, y;
  int channels_in_file;

  stbi_set_flip_vertically_on_load(true);
  stbi_uc *image_data =
      stbi_load(image_path.c_str(), &x, &y, &channels_in_file, 3);

  CreateTexture(image_data, glm::uvec2(x, y));

  stbi_image_free(image_data);
}

void ImageRenderer::CreateTexture(const unsigned char *image_data,
                                  glm::uvec2 size) {
  gl::TextureCreateInfo create_info;
  create_info.format = GL_RGB;
  create_info.internal_format = GL_RGB;
  create_info.min_filter = GL_LINEAR;
  create_info.mag_filter = GL_LINEAR;
  create_info.wrap_s = GL_CLAMP_TO_EDGE;
  create_info.wrap_t = GL_CLAMP_TO_EDGE;
  create_info.type = GL_UNSIGNED_BYTE;

  m_texture.Create(size, create_info, image_data);
}

// clang-format off
const std::array<ImageRenderer::Vertex, 4> ImageRenderer::s_vertices = {
  Vertex{{-1, -1, 0}, {0, 0}},
  Vertex{{ 1, -1, 0}, {1, 0}},
  Vertex{{ 1,  1, 0}, {1, 1}},
  Vertex{{-1,  1, 0}, {0, 1}}
};
// clang-format on

// clang-format off
const std::vector<gl::VAO::VertexField> ImageRenderer::s_vertex_fields = {
        {2, GL_FLOAT, offsetof(Vertex, pos)},
        {2, GL_FLOAT, offsetof(Vertex, uv)},
};
// clang-format on

// clang-format off
const std::array<GLuint, 6> ImageRenderer::s_indices = {
  0, 1, 2, 
  0, 2, 3
};
// clang-format on
