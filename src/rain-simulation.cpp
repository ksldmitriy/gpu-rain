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

std::ostream &operator<<(std::ostream &os, glm::fvec2 vec) {
  os << "(" << vec.x << ", " << vec.y << ")";

  return os;
}

std::ostream &operator<<(std::ostream &os, glm::fvec3 vec) {
  os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";

  return os;
}

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
                            glm::fvec2 droplet_size, glm::fvec2 splash_size) {
  m_droplets_count = droplets_count;
  m_framebuffer_size = framebuffer_size;
  m_droplet_size = droplet_size;
  m_splash_size = splash_size;

  CreateProjectionMatrix();
  CalculateSpawnPositions();
  CalculateSpawnRation();

  CreateComputeBuffers();
  CreateRenderObjects();

  CreateDepthTexture();
  CreateDropTexture();

  glm::fvec4 test_nl = m_transform_matrix * glm::fvec4(m_spawn_nl, 1);
  glm::fvec4 test_nr = m_transform_matrix * glm::fvec4(m_spawn_nr, 1);
  glm::fvec4 test_fl = m_transform_matrix * glm::fvec4(m_spawn_fl, 1);
  glm::fvec4 test_fr = m_transform_matrix * glm::fvec4(m_spawn_fr, 1);
}

void RainSimulation::Run(float delta_time) {
  gl::Program &program = ProgramsLibrary::GetRainComputeProgram();
  program.SetUniformInt("u_drops_count", m_droplets_count);
  program.SetUniformFloat("u_kill_plane", s_kill_plane);
  program.SetUniformFloat("u_top_down_camera_y", s_top_down_camera_y);
  program.SetUniformFloat("u_speed", s_droplet_speed);
  program.SetUniformFloat("u_delta_time", delta_time);

  program.SetUniformFVec3("u_spawn_nl", m_spawn_nl);
  program.SetUniformFVec3("u_spawn_nr", m_spawn_nr);
  program.SetUniformFVec3("u_spawn_fl", m_spawn_fl);
  program.SetUniformFVec3("u_spawn_fr", m_spawn_fr);

  program.SetUniformFVec2("u_spawn_nl_uv", m_spawn_nl_uv);
  program.SetUniformFVec2("u_spawn_nr_uv", m_spawn_nr_uv);
  program.SetUniformFVec2("u_spawn_fl_uv", m_spawn_fl_uv);
  program.SetUniformFVec2("u_spawn_fr_uv", m_spawn_fr_uv);

  program.SetUniformFloat("u_spawn_ratio", m_spawn_ratio);

  program.SetUniformFloat("u_spawn_y_range", 0.f);
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
  program.SetUniformFloat("u_near_plane", 0);
  program.SetUniformFloat("u_far_plane", m_spawn_nl.z - m_spawn_fl.z);
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

  glm::fvec2 droplet_gl_size = m_droplet_size / glm::fvec2(m_framebuffer_size);
  glm::fvec2 splash_gl_size = m_splash_size / glm::fvec2(m_framebuffer_size);

  for (DropletVertex &vertex : droplet_vertices) {
    vertex.fall_pos *= droplet_gl_size;
    vertex.splash_pos *= splash_gl_size;
  }

  m_vbo.Create(4 * sizeof(DropletVertex), GL_STATIC_DRAW,
               droplet_vertices.data());

  m_vao.Create(s_droplet_vertex_attribs, sizeof(DropletVertex));
  m_vbo.Bind();
  m_vao.PointAttribs();

  m_ebo.Create(6, GL_STATIC_DRAW, s_droplet_indices);
}

void RainSimulation::CreateProjectionMatrix() {
  float vfov = CalculateVerticalFov();

  glm::mat4 view_matrix = glm::lookAt(glm::fvec3(0, 0, 0), glm::fvec3(0, 0, -1),
                                      glm::fvec3(0, 1, 0));

  glm::mat4 projection_matrix =
      glm::perspective(vfov, 16.0f / 9.0f, s_min_z, s_max_z);

  m_transform_matrix = projection_matrix * view_matrix;
}

void RainSimulation::CalculateSpawnPositions() {
  float vfov = CalculateVerticalFov();

  // x
  float x_tan = tan(s_hfov / 2.f);

  m_spawn_nl.x = -s_min_z * x_tan;
  m_spawn_nr.x = s_min_z * x_tan;

  m_spawn_fl.x = -s_max_z * x_tan;
  m_spawn_fr.x = s_max_z * x_tan;

  // y
  float y_tan = tan(vfov / 2.f);
  const float const_y = 10.f;

  m_spawn_nl.y = const_y;
  m_spawn_nr.y = const_y;

  m_spawn_fl.y = const_y;
  m_spawn_fr.y = const_y;

  // z
  m_spawn_nl.z = -s_min_z;
  m_spawn_nr.z = -s_min_z;

  m_spawn_fl.z = -s_max_z;
  m_spawn_fr.z = -s_max_z;

  // uv x
  float near_x_uv = (s_min_z * x_tan) / 28;
  m_spawn_nl_uv.x = 0.5f - near_x_uv;
  m_spawn_nr_uv.x = 0.5f + near_x_uv;

  float far_x_uv = (s_max_z * x_tan) / 28;
  m_spawn_fl_uv.x = 0.5f - far_x_uv;
  m_spawn_fr_uv.x = 0.5f + far_x_uv;

  // uv y
  m_spawn_nl_uv.y = 1;
  m_spawn_nr_uv.y = 1;

  m_spawn_fl_uv.y = 0;
  m_spawn_fr_uv.y = 0;
}

static float triangle_area(glm::fvec3 p1, glm::fvec3 p2, glm::fvec3 p3) {
  glm::fvec3 v1 = p2 - p1;
  glm::fvec3 v2 = p3 - p1;

  glm::fvec3 cross = glm::cross(v1, v2);

  float magnitude = glm::length(cross);

  return 0.5f * magnitude;
}

void RainSimulation::CalculateSpawnRation() {
  float area1 = triangle_area(m_spawn_fr, m_spawn_nl, m_spawn_fl);
  float area2 = triangle_area(m_spawn_fr, m_spawn_nl, m_spawn_nr);

  m_spawn_ratio = area1 / (area1 + area2);
}

float RainSimulation::CalculateVerticalFov() {
  float aspect_ration = m_framebuffer_size.y / (float)m_framebuffer_size.x;
  float vertical_fov = 2 * atanf(tanf(s_hfov / 2.f) * aspect_ration);

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
  static std::mt19937_64 mt;
  static const float y_range =
      (((m_spawn_fl.y + m_spawn_fr.y) / 2.f) - s_kill_plane) * 3.f;

  std::uniform_real_distribution<float> dt(0, 1);
  float r_p = dt(mt);

  float r1 = dt(mt);
  float r2 = dt(mt);
  float r3 = dt(mt);

  float sqrt_r1 = sqrt(r1);
  float a = 1.f - sqrt_r1;
  float b = sqrt_r1 * (1.f - r2);
  float c = sqrt_r1 * r2;

  glm::fvec3 p3;
  glm::fvec2 p3_uv;
  if (r_p < m_spawn_ratio) {
    p3 = m_spawn_fl;
    p3_uv = m_spawn_fl_uv;
  } else {
    p3 = m_spawn_nr;
    p3_uv = m_spawn_nr_uv;
  }

  drop.pos = (m_spawn_nl * a) + (p3 * b) + (m_spawn_fr * c);
  drop.pos.y += y_range * r3;

  drop.top_down_uv = (m_spawn_nl_uv * a) + (p3_uv * b) + (m_spawn_fr_uv * c);

  drop.splash_time = 0;
}

// clang-format off
const std::array<RainSimulation::DropletVertex, 4> RainSimulation::s_droplet_vertices = {
  {
	{
	  glm::fvec2(-1, 0),
	  glm::fvec2(-1, 0),
	  glm::fvec2(0, 0)
	},
	{
	  glm::fvec2(1, 0),
	  glm::fvec2(1, 0),
	  glm::fvec2(1, 0)
	},
	{
	  glm::fvec2(1, 1),
	  glm::fvec2(1, 2),
	  glm::fvec2(1, 1),
	},
	{
	  glm::fvec2(-1, 2),
	  glm::fvec2(-1, 2),
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

static constexpr float cam_z = -16;
static constexpr float td_cam_size = 28;

// scene
const float RainSimulation::s_min_z = -5.22811 - cam_z;
const float RainSimulation::s_max_z = RainSimulation::s_min_z + td_cam_size;
const float RainSimulation::s_hfov = glm::radians(60.f);
const float RainSimulation::s_top_down_camera_y = 9 + 6.5;

const float RainSimulation::s_near_plane = s_min_z;
const float RainSimulation::s_kill_plane = -4.5;

// drop
const float RainSimulation::s_min_drop_size = 0.1f;
const float RainSimulation::s_max_drop_size = 1.f;
const glm::fvec3 RainSimulation::s_drop_color = glm::fvec3(0.8f, 0.8f, 0.95f);
const float RainSimulation::s_droplet_speed = 18.f;
const float RainSimulation::s_splash_time = 0.18f;
