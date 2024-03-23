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
              glm::fvec2 droplet_size, glm::fvec2 splash_size);

  void Run(float delta_time);
  void Draw();

private:
  void CreateComputeBuffers();
  void CreateRenderObjects();

  void CreateProjectionMatrix();
  void CalculateSpawnPositions();
  void CalculateSpawnRation();
  float CalculateVerticalFov();

  void CreateDepthTexture();
  void CreateDropTexture();

  void GenerateRandomDrop(RainDroplet &drop);

private:
  size_t m_droplets_count;
  glm::uvec2 m_framebuffer_size;
  glm::fvec2 m_droplet_size;
  glm::fvec2 m_splash_size;

  gl::SSBO m_ssbo;

  gl::VAO m_vao;
  gl::VBO m_vbo;
  gl::EBO m_ebo;
  gl::ACBO m_acbo;

  glm::mat4 m_transform_matrix;

  gl::Texture m_main_depth;
  gl::Texture m_top_down_depth;
  gl::Texture m_droplet_texture;
  gl::Texture m_droplet_splash_texture;

  glm::fvec3 m_spawn_nl;
  glm::fvec3 m_spawn_nr;
  glm::fvec3 m_spawn_fl;
  glm::fvec3 m_spawn_fr;

  glm::fvec2 m_spawn_nl_uv;
  glm::fvec2 m_spawn_nr_uv;
  glm::fvec2 m_spawn_fl_uv;
  glm::fvec2 m_spawn_fr_uv;

  float m_spawn_ratio;

  static const std::array<DropletVertex, 4> s_droplet_vertices;
  static const std::vector<gl::VAO::VertexField> s_droplet_vertex_attribs;
  static const gl::GLuint s_droplet_indices[6];

  static constexpr uint s_compute_group_size = 64;

  // scene
  static const float s_min_z;
  static const float s_max_z;
  static const float s_depth_widht;
  static const float s_hfov;
  static const float s_top_down_camera_y;

  static const float s_near_plane;
  static const float s_kill_plane;

  static const float s_depth_texture_min_z;
  static const float s_depth_texture_max_z;

  // drop
  static const float s_min_drop_size;
  static const float s_max_drop_size;
  static const float s_droplet_speed;
  static const float s_splash_time;

  static const char* s_main_depth_image_path;
  static const char* s_top_down_depth_image_path;
};
