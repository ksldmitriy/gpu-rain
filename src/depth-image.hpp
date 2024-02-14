#pragma once
#include <ImfArray.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <memory.h>
#include <vector>

namespace fs = std::filesystem;

class DepthImage {
public:
  DepthImage();
  DepthImage(DepthImage &) = delete;
  DepthImage &operator=(DepthImage &) = delete;
  ~DepthImage();

  void Open(fs::path filepath);
  void Free();

  float* GetData();
  glm::uvec2 GetSize();
  
  bool IsOpen();

private:
private:
  Imf::Array2D<float> m_data;
  glm::uvec2 m_size;
};
