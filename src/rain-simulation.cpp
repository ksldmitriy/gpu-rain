#include "rain-simulation.hpp"

#include "depth-image.hpp"
#include "programs-library.hpp"

#include "glad/gl.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>
#include <stb/stb_image.h>
#include <vector>

RainSimulation::RainSimulation() {
}

RainSimulation::RainSimulation(RainSimulation &&simulation) {
  *this = std::move(simulation);
}

RainSimulation &RainSimulation::operator=(RainSimulation &&simulation) {
  simulation.m_ssbo = std::move(m_ssbo);
  simulation.m_droplets_count = simulation.m_droplets_count;

  simulation.m_droplets_count = 0;

  return *this;
}

RainSimulation::~RainSimulation() {
  m_ssbo.Destroy();
  m_vbo.Free();
  m_vao.Destroy();
  m_ebo.Destroy();
}

void RainSimulation::Create(size_t droplets_count, glm::uvec2 framebuffer_size,
                            float droplet_size) {
  m_droplets_count = droplets_count;
  m_framebuffer_size = framebuffer_size;
  m_droplet_size = droplet_size;

  CalculatePositions();

  CreateComputeBuffers();

  CreateRenderObjects();

  CreateDepthTexture();

  CreateDropTexture();
}

void RainSimulation::Run(float delta_time) {
  gl::Program &program = ProgramsLibrary::GetRainComputeProgram();
  program.SetUniformInt("u_drops_count", m_droplets_count);
  program.SetUniformFloat("u_kill_plane", m_kill_plane);
  program.SetUniformFloat("u_top_down_camera_y", 15.f);
  program.SetUniformFloat("u_speed", s_droplet_speed);
  program.SetUniformFloat("u_delta_time", delta_time);

  program.SetUniformFVec3("u_spawn_p1", m_spawn_camera);
  program.SetUniformFVec3("u_spawn_p2", m_spawn_left);
  program.SetUniformFVec3("u_spawn_p3", m_spawn_right);
  program.SetUniformFloat("u_spawn_y_range", 9.f);
  program.SetUniformFloat("u_splash_duration", s_splash_time);

  program.Use();

  m_ssbo.Bind(0);
  m_acbo.Bind(1);

  m_top_down_depth.Bind();

  uint compute_groups_count =
      (m_droplets_count + s_compute_group_size - 1) / s_compute_group_size;

  glDispatchCompute(compute_groups_count, 1, 1);
}

void RainSimulation::Draw() {
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  gl::Program &program = ProgramsLibrary::GetRainDrawProgram();
  program.SetUniformMat4f("u_transform_matrix", m_transform_matrix);
  program.SetUniformFloat("u_near_plane", s_near_plane);
  program.SetUniformFloat("u_far_plane", s_far_plane);
  program.SetUniformFloat("u_min_drop_size", s_min_drop_size);
  program.SetUniformFloat("u_max_drop_size", s_max_drop_size);
  program.SetUniformFVec3("u_drop_color", s_drop_color);

  program.Use();

  m_main_depth.Bind(1);
  m_droplet_texture.Bind(2);
  m_droplet_splash_texture.Bind(3);

  m_vao.Bind();
  m_ebo.Bind();

  m_ssbo.Bind(0);

  glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0,
                          m_droplets_count);
}

void RainSimulation::CreateComputeBuffers() {
  std::vector<RainDroplet> data(m_droplets_count);

  for (int i = 0; i < m_droplets_count; i++) {
    RainDroplet droplet;

    GenerateRandomDrop(droplet);

    data[i] = droplet;
  }

  m_ssbo.Create(m_droplets_count * sizeof(RainDroplet), GL_DYNAMIC_DRAW,
                data.data());

  GLuint empty_acbo_data = 0;
  m_acbo.Create(1, GL_STATIC_DRAW, &empty_acbo_data);
}

void RainSimulation::CreateRenderObjects() {
  std::array<DropletVertex, 4> droplet_vertices = s_droplet_vertices;

  glm::fvec2 droplet_gl_size = glm::fvec2(m_droplet_size, m_droplet_size) /
                               glm::fvec2(m_framebuffer_size);

  for (DropletVertex &vertex : droplet_vertices) {
    vertex.fall_pos *= droplet_gl_size;
    vertex.splash_pos *= droplet_gl_size;
  }

  m_vbo.Create(4 * sizeof(DropletVertex), GL_STATIC_DRAW,
               droplet_vertices.data());

  m_vao.Create(s_droplet_vertex_attribs, sizeof(DropletVertex));
  m_vbo.Bind();
  m_vao.PointAttribs();

  m_ebo.Create(6, GL_STATIC_DRAW, s_droplet_indices);
}

void RainSimulation::CalculatePositions() {
  float vertical_fov = CalculateVerticalFov();

  m_spawn_min.x = s_camera_pos.x - (s_far_plane * tan(s_fov / 2.f));
  m_spawn_max.x = s_camera_pos.x + (s_far_plane * tan(s_fov / 2.f));

  m_spawn_min.y = s_camera_pos.y + (s_far_plane * tan(vertical_fov / 2.f));
  m_spawn_max.y = m_spawn_min.y + 0.0;
  m_kill_plane =
      s_camera_pos.y - (s_far_plane * tan(vertical_fov / 2.f)) - 0.1f;

  m_spawn_min.z = s_camera_pos.z + 0.2f;
  m_spawn_max.z = s_max_z;

  m_spawn_camera = s_camera_pos + glm::fvec3(0, 4, 0);

  m_spawn_left.x = s_camera_pos.x - (s_far_plane * tan(s_fov / 2.f));
  m_spawn_right.x = s_camera_pos.x + (s_far_plane * tan(s_fov / 2.f));

  m_spawn_left.y = m_spawn_camera.y;
  m_spawn_right.y = m_spawn_camera.y;

  m_spawn_left.z = s_max_z;
  m_spawn_right.z = s_max_z;

  glm::mat4 view_matrix = glm::lookAt(
      s_camera_pos, s_camera_pos + glm::fvec3(0, 0, 1), glm::fvec3(0, 1, 0));

  glm::mat4 projection_matrix =
      glm::perspective(vertical_fov, 16.0f / 9.0f, 0.01f, s_far_plane);

  m_transform_matrix = projection_matrix * view_matrix;
}

float RainSimulation::CalculateVerticalFov() {
  float aspect_ration = m_framebuffer_size.y / (float)m_framebuffer_size.x;
  float vertical_fov = 2 * atanf(tanf(s_fov / 2.f) * aspect_ration);

  return vertical_fov;
}

void RainSimulation::CreateDepthTexture() {
  DepthImage main_image;
  main_image.Open("media/main-depth.exr");
  if (!main_image.IsOpen()) {
    throw std::runtime_error("cant open main depth image");
  }

  DepthImage top_down_image;
  top_down_image.Open("media/top-down-depth.exr");
  if (!top_down_image.IsOpen()) {
    throw std::runtime_error("cant open top down depth image");
  }

  gl::TextureCreateInfo depth_create_info;
  depth_create_info.format = GL_RED;
  depth_create_info.internal_format = GL_R32F;
  depth_create_info.min_filter = GL_NEAREST;
  depth_create_info.mag_filter = GL_NEAREST;
  depth_create_info.wrap_s = GL_CLAMP_TO_EDGE;
  depth_create_info.wrap_t = GL_CLAMP_TO_EDGE;
  depth_create_info.type = GL_FLOAT;

  m_main_depth.Create(main_image.GetSize(), depth_create_info,
                      main_image.GetData());

  m_top_down_depth.Create(top_down_image.GetSize(), depth_create_info,
                          top_down_image.GetData());
}

void RainSimulation::CreateDropTexture() {
  int x, y;
  int channels_in_file;

  gl::TextureCreateInfo create_info;
  create_info.format = GL_RGBA;
  create_info.internal_format = GL_RGBA;
  create_info.min_filter = GL_LINEAR;
  create_info.mag_filter = GL_LINEAR;
  create_info.wrap_s = GL_CLAMP_TO_EDGE;
  create_info.wrap_t = GL_CLAMP_TO_EDGE;
  create_info.type = GL_UNSIGNED_BYTE;

  stbi_set_flip_vertically_on_load(true);
  stbi_uc *image_data =
      stbi_load("media/drop.png", &x, &y, &channels_in_file, 4);

  m_droplet_texture.Create(glm::uvec2(x, y), create_info, image_data);

  stbi_image_free(image_data);

  stbi_set_flip_vertically_on_load(true);
  image_data = stbi_load("media/splash.png", &x, &y, &channels_in_file, 4);

  m_droplet_splash_texture.Create(glm::uvec2(x, y), create_info, image_data);

  stbi_image_free(image_data);
}

void RainSimulation::GenerateRandomDrop(RainDroplet &drop) {
  static const glm::fvec2 spawn_uv_p1(1.f, 0.5f);
  static const glm::fvec2 spawn_uv_p2(0.f, 0.f);
  static const glm::fvec2 spawn_uv_p3(0.f, 1.f);

  static std::mt19937_64 mt;
  static const float y_range =
      (((m_spawn_left.y + m_spawn_right.y) / 2.f) - m_kill_plane) * 3.f;

  std::uniform_real_distribution<float> dt(0, 1);
  float r1 = dt(mt);
  float r2 = dt(mt);
  float r3 = dt(mt);

  float sqrt_r1 = sqrt(r1);
  float a = 1.f - sqrt_r1;
  float b = sqrt_r1 * (1.f - r2);
  float c = sqrt_r1 * r2;

  drop.pos = (m_spawn_camera * a) + (m_spawn_left * b) + (m_spawn_right * c);
  drop.pos.y += y_range * r3;

  drop.top_down_uv = (spawn_uv_p1 * a) + (spawn_uv_p2 * b) + (spawn_uv_p3 * c);

  drop.splash_time = 0;
}

static constexpr float drop_fall_height = 16.f;
static constexpr float drop_splash_width = 3.5f;
static constexpr float drop_splash_height = 2.7f;

// clang-format off
const std::array<RainSimulation::DropletVertex, 4> RainSimulation::s_droplet_vertices = {
  {
	{
	  glm::fvec2(-1, 0),
	  glm::fvec2(-drop_splash_width, 0),
	  glm::fvec2(0, 0)
	},
	{
	  glm::fvec2(1, 0),
	  glm::fvec2(drop_splash_width, 0),
	  glm::fvec2(1, 0)
	},
	{
	  glm::fvec2(1, drop_fall_height * 2),
	  glm::fvec2(drop_splash_width, drop_splash_height * 2),
	  glm::fvec2(1, 1),
	},
	{
	  glm::fvec2(-1, drop_fall_height * 2),
	  glm::fvec2(-drop_splash_width, drop_splash_height * 2),
	  glm::fvec2(0, 1)
	}
  }
};
// clang-format on

// clang-format off
const std::vector<gl::VAO::VertexField> RainSimulation::s_droplet_vertex_attribs = {
  {2, GL_FLOAT, offsetof(DropletVertex, fall_pos)},
  {2, GL_FLOAT, offsetof(DropletVertex, splash_pos)},
  {2, GL_FLOAT, offsetof(DropletVertex, uv)}
};
// clang-format on

const gl::GLuint RainSimulation::s_droplet_indices[6] = {0, 1, 2, 0, 2, 3};

const float RainSimulation::s_max_z = 5;
const float RainSimulation::s_fov = glm::radians(60.f);
const glm::fvec3 RainSimulation::s_camera_pos = glm::fvec3(0, 3, -10);
const float RainSimulation::s_near_plane = 0.01f;
const float RainSimulation::s_far_plane = s_max_z - s_camera_pos.z;
const float RainSimulation::s_min_drop_size = 0.35f;
const float RainSimulation::s_max_drop_size = 1.f;
const glm::fvec3 RainSimulation::s_drop_color = glm::fvec3(0.8f, 0.8f, 0.95f);
const float RainSimulation::s_droplet_speed = 12.f;
const float RainSimulation::s_splash_time = 0.18f;
