#pragma once
#include <glm/glm.hpp>

namespace gl {

typedef unsigned int GLuint, GLenum;
typedef int GLint;

struct TextureCreateInfo {
  GLint internal_format;
  GLuint format;
  GLint min_filter, mag_filter;
  GLint wrap_s, wrap_t;
};

class Texture {
public:
  Texture();
  Texture(Texture &) = delete;
  Texture &operator=(Texture &) = delete;
  Texture(Texture &&texture);
  Texture &operator=(Texture &&texture);
  ~Texture();

  void Create(glm::uvec2 size, TextureCreateInfo &create_info,
              const void *data = nullptr);
  void Delete();

  GLuint GetHandle();
  void Bind() const;
  glm::uvec2 GetSize() const;

private:
  GLuint m_handle = 0;
  glm::uvec2 m_size;
};

} // namespace gl
