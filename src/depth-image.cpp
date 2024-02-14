#include "depth-image.hpp"

#include <ImfChannelList.h>
#include <ImfInputFile.h>
#include <ImfRgbaFile.h>

#include <stdexcept>
#include <string>
#include <utility>

static Imf::Slice CreateChannelSlice(Imf::Array2D<float> &data, Imath::Box2i dw,
                                     glm::ivec2 size);

DepthImage::DepthImage() {
}

DepthImage::~DepthImage() {
  if (!IsOpen()) {
    return;
  }

  Free();
}

void DepthImage::Open(fs::path filepath) {
  Imf::InputFile file(filepath.c_str());
  Imf::Header header = file.header();

  Imath::Box2i dw = header.dataWindow();
  m_size.x = dw.max.x - dw.min.x + 1;
  m_size.y = dw.max.y - dw.min.y + 1;

  m_data.resizeErase(m_size.y, m_size.x);
  Imf::Slice slice = CreateChannelSlice(m_data, dw, m_size);

  Imf::ChannelList channel_list = header.channels();
  const char *channel_name = channel_list.begin().name();

  Imf::FrameBuffer framebuffer;
  framebuffer.insert(channel_name, slice);

  file.setFrameBuffer(framebuffer);

  file.readPixels(dw.min.y, dw.max.y);
}

void DepthImage::Free() {
  m_data.resizeErase(0, 0);
  m_size = glm::uvec2(0, 0);
}

float *DepthImage::GetData() {
  return &m_data[0][0];
}

glm::uvec2 DepthImage::GetSize() {
  return m_size;
}

bool DepthImage::IsOpen() {
  return (m_data.height() && m_data.width());
}

static Imf::Slice CreateChannelSlice(Imf::Array2D<float> &data, Imath::Box2i dw,
                                     glm::ivec2 size) {
  Imf::Slice slice(Imf::PixelType::FLOAT,
                   (char *)(&data[0][0] - dw.min.x - dw.min.y * size.x),
                   sizeof(data[0][0]), sizeof(data[0][0]) * size.x, 1, 1, 0.0);

  return slice;
}
