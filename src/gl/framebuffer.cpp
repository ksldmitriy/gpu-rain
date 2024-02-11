#include "framebuffer.hpp"

#include "texture.hpp"

#include <glad/gl.h>
#include <stdexcept>

namespace gl {

Framebuffer::Framebuffer() {
}

Framebuffer::Framebuffer(Framebuffer &&framebuffer) {
  *this = std::move(framebuffer);
}

Framebuffer &Framebuffer::operator=(Framebuffer &&framebuffer) {
  if (m_handle) {
    throw std::runtime_error("trying to move framebuffer to existing one");
  }

  m_handle = framebuffer.m_handle;
  framebuffer.m_handle = 0;

  return *this;
}

Framebuffer::~Framebuffer() {
  if (!m_handle) {
    return;
  }

  Delete();
}

void Framebuffer::Create() {
  if (m_handle) {
    throw std::runtime_error("trying to create framebuffer over existing one");
  }

  glGenFramebuffers(1, &m_handle);
  glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
}

void Framebuffer::AttachTexture(Texture &texture, GLenum target) {
  glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
  glFramebufferTexture2D(GL_FRAMEBUFFER, target, GL_TEXTURE_2D,
                         texture.GetHandle(), 0);
}

void Framebuffer::Delete() {
  if (!m_handle) {
    throw std::runtime_error("trying to delete non existing framebuffer");
  }

  glDeleteFramebuffers(1, &m_handle);
  m_handle = 0;
}

void Framebuffer::Bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
}

} // namespace gl
