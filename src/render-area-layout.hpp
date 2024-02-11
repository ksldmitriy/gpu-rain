#pragma once
#include "gl/vao.hpp"
#include "gl/vbo.hpp"
#include "gl/ebo.hpp"

#include <glm/glm.hpp>
#include <vector>


struct RenderAreaLayout {
  gl::VBO vbo;
  gl::VAO vao;
  gl::EBO ebo;
  size_t index_count;
};
