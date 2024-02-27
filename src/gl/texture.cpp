#include "texture.hpp"

#include <glad/gl.h>
#include <stdexcept>

namespace gl {

Texture::Texture() {
}

Texture::Texture(Texture &&texture) {
  *this = std::move(texture);
}

Texture &Texture::operator=(Texture &&texture) {
  if (m_handle) {
    throw std::runtime_error("trying to move texture to existing one");
  }

  m_handle = texture.m_handle;
  m_size = texture.m_size;

  texture.m_handle = 0;

  return *this;
}

Texture::~Texture() {
  if (!m_handle) {
    return;
  }

  Delete();
}

void Texture::Create(glm::uvec2 size, const TextureCreateInfo &create_info,
                     const void *data) {
  if (m_handle) {
    throw std::runtime_error("trying to create texture over existing one");
  }

  m_size = size;

  glGenTextures(1, &m_handle);
  glBindTexture(GL_TEXTURE_2D, m_handle);

  glTexImage2D(GL_TEXTURE_2D, 0, create_info.internal_format, size.x, size.y, 0,
               create_info.format, create_info.type, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, create_info.min_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, create_info.mag_filter);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, create_info.wrap_s);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, create_info.wrap_t);
}

void Texture::Delete() {
  if (!m_handle) {
    throw std::runtime_error("trying to delete non existing texture");
  }

  glDeleteTextures(1, &m_handle);
}

GLuint Texture::GetHandle() {
  return m_handle;
}

void Texture::Bind(size_t index) const {
  glActiveTexture(GL_TEXTURE0 + index);
  glBindTexture(GL_TEXTURE_2D, m_handle);
}

glm::uvec2 Texture::GetSize() const {
  return m_size;
}

} // namespace gl
