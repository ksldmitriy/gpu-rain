#include "ms-texture.hpp"

#include <glad/gl.h>
#include <stdexcept>

namespace gl {

MsTexture::MsTexture() {
}

MsTexture::MsTexture(MsTexture &&texture) {
  *this = std::move(texture);
}

MsTexture &MsTexture::operator=(MsTexture &&texture) {
  if (m_handle) {
    throw std::runtime_error("trying to move texture to existing one");
  }

  m_handle = texture.m_handle;
  m_size = texture.m_size;

  texture.m_handle = 0;

  return *this;
}

MsTexture::~MsTexture() {
  if (!m_handle) {
    return;
  }

  Delete();
}

void MsTexture::Create(glm::uvec2 size,
                       const MsTextureCreateInfo &create_info) {
  if (m_handle) {
    throw std::runtime_error("trying to create texture over existing one");
  }

  m_size = size;

  glGenTextures(1, &m_handle);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_handle);

  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, create_info.samples,
                          create_info.internal_format, size.x, size.y, true);
}

void MsTexture::Delete() {
  if (!m_handle) {
    throw std::runtime_error("trying to delete non existing texture");
  }

  glDeleteTextures(1, &m_handle);
}

GLuint MsTexture::GetHandle() {
  return m_handle;
}

void MsTexture::Bind(size_t index) const {
  glActiveTexture(GL_TEXTURE0 + index);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_handle);
}

glm::uvec2 MsTexture::GetSize() const {
  return m_size;
}

} // namespace gl
