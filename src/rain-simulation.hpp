#pragma once
#include "gl/acbo.hpp"
#include "gl/ebo.hpp"
#include "gl/ssbo.hpp"
#include "gl/texture.hpp"
#include "gl/vao.hpp"
#include "gl/vbo.hpp"

#include <array>
#include <glm/glm.hpp>

#pragma pack(1)
struct RainDroplet {
  // alg* variables are needed for aligment
  glm::fvec3 pos;
  float splash_time;
  glm::fvec2 top_down_uv;
  float alg2[2];
};
#pragma pack(0)

class RainSimulation {
private:
  struct DropletVertex {
    glm::fvec2 fall_pos;
    glm::fvec2 splash_pos;
    glm::fvec2 uv;
  };

public:
  RainSimulation();
  RainSimulation(RainSimulation &) = delete;
  RainSimulation &operator=(RainSimulation &) = delete;
  RainSimulation(RainSimulation &&simulation);
  RainSimulation &operator=(RainSimulation &&simulation);
  ~RainSimulation();

  void Create(size_t droplets_count, glm::uvec2 framebuffer_size,
              float droplet_size);

  void Run(float delta_time);
  void Draw();

private:
  void CreateComputeBuffers();
  void CreateRenderObjects();

  void CalculatePositions();
  float CalculateVerticalFov();

  void CreateDepthTexture();
  void CreateDropTexture();

  void GenerateRandomDrop(RainDroplet &drop);

private:
  size_t m_droplets_count;
  glm::uvec2 m_framebuffer_size;
  float m_droplet_size;

  gl::SSBO m_ssbo;

  gl::VAO m_vao;
  gl::VBO m_vbo;
  gl::EBO m_ebo;
  gl::ACBO m_acbo;

  glm::fvec3 m_spawn_min;
  glm::fvec3 m_spawn_max;
  glm::mat4 m_transform_matrix;
  float m_kill_plane;

  gl::Texture m_main_depth;
  gl::Texture m_top_down_depth;
  gl::Texture m_droplet_texture;
  gl::Texture m_droplet_splash_texture;

  glm::fvec3 m_spawn_camera;
  glm::fvec3 m_spawn_left;
  glm::fvec3 m_spawn_right;

  static const std::array<DropletVertex, 4> s_droplet_vertices;
  static const std::vector<gl::VAO::VertexField> s_droplet_vertex_attribs;
  static const gl::GLuint s_droplet_indices[6];

  static constexpr uint s_compute_group_size = 64;

  static const float s_max_z;
  static const float s_fov;
  static const glm::fvec3 s_camera_pos;
  static const float s_near_plane;
  static const float s_far_plane;
  static const float s_min_drop_size;
  static const float s_max_drop_size;
  static const glm::fvec3 s_drop_color;
  static const float s_droplet_speed;
  static const float s_splash_time;
};
