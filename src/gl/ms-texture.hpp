#pragma once
#include <glm/glm.hpp>

namespace gl {

typedef unsigned int GLuint, GLenum;
typedef int GLint, GLsizei;

struct MsTextureCreateInfo {
  GLsizei samples;
  GLint internal_format;
  bool fixed_sample_locations;
};

class MsTexture {
public:
  MsTexture();
  MsTexture(MsTexture &) = delete;
  MsTexture &operator=(MsTexture &) = delete;
  MsTexture(MsTexture &&texture);
  MsTexture &operator=(MsTexture &&texture);
  ~MsTexture();

  void Create(glm::uvec2 size, const MsTextureCreateInfo &create_info);
  void Delete();

  GLuint GetHandle();
  void Bind(size_t index = 0) const;
  glm::uvec2 GetSize() const;

private:
  GLuint m_handle = 0;
  glm::uvec2 m_size;
};

} // namespace gl
