#pragma once
#include <glm/glm.hpp>

namespace gl {

typedef unsigned int GLuint, GLenum;

class Texture;
class MsTexture;

class Framebuffer {
public:
  Framebuffer();
  Framebuffer(Framebuffer &) = delete;
  Framebuffer &operator=(Framebuffer &) = delete;
  Framebuffer(Framebuffer &&framebuffer);
  Framebuffer &operator=(Framebuffer &&framebuffer);
  ~Framebuffer();

  void Create();
  void AttachTexture(Texture &texture, GLenum target);
  void AttachMsTexture(MsTexture &texture, GLenum target);
  void Delete();

  GLuint GetHande();
  void Bind() const;

private:
  GLuint m_handle = 0;
};

} // namespace gl
