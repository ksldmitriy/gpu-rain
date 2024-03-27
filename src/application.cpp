#include "application.hpp"

#include "programs-library.hpp"
#include "render-area-layout-constructor.hpp"

#include <glad/gl.h>
#include <stb/stb_image.h>

#include <ratio>

Application::Application() {
}

int Application::Run() {
  Init();

  MainLoop();

  Cleanup();

  return 0;
}

void Application::Init() {
  m_window.Open();

  m_window.CreateGLContext();

  m_window.GetMonitorDimensions();

  CreateMsFramebuffer();
  CreatePostFramebuffer();

  ProgramsLibrary::CreatePrograms();

  std::vector<MonitorDimensions> monitor_dimentions =
      m_window.GetMonitorDimensions();

  RenderAreaLayoutConstructor render_area_constructor;
  render_area_constructor.ConstructLayout(monitor_dimentions,
                                          s_framebuffer_size,
                                          m_window.GetSize(), m_render_area);

  m_bg_image_renderer.Create(s_test_media_path);

  m_rain_simulation.Create(s_drops_count, s_framebuffer_size, s_drop_size,
                           s_splash_size);

  m_prev_frame_time = hires_clock::now();
  m_delta_time = 0;
}

void Application::MainLoop() {
  while (true) {
    UpdateDeltaTime();

    m_rain_simulation.Run(m_delta_time);

    DrawToFrambuffer();
    DrawToScreen();

    m_window.SwapBuffers();
  }
}

void Application::DrawToFrambuffer() {
  m_ms_framebuffer.Bind();
  glEnable(GL_MULTISAMPLE);
  SetViewport(s_framebuffer_size);

  glClearColor(1.0f, 0.0f, 0.0f, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_bg_image_renderer.Draw();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  m_rain_simulation.Draw();
}

void Application::DrawToScreen() {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ms_framebuffer.GetHande());
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_post_framebuffer.GetHande());

  glBlitFramebuffer(0, 0, s_framebuffer_size.x, s_framebuffer_size.y, 0, 0,
                    s_framebuffer_size.x, s_framebuffer_size.y,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  SetViewport(m_window.GetSize());

  glClearColor(0.0f, 0.0f, 1.0f, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ProgramsLibrary::GetTextureProgram().Use();
  m_render_area.vao.Bind();
  m_render_area.ebo.Bind();

  m_post_color_texture.Bind();

  glDrawElements(GL_TRIANGLES, m_render_area.index_count, GL_UNSIGNED_INT, 0);
}

void Application::Cleanup() {
  m_window.Close();
}

void Application::UpdateDeltaTime() {
  time_point_t current_frame_time = hires_clock::now();

  duration_t delta_duration = current_frame_time - m_prev_frame_time;
  float delta_time_us =
      chrono::duration_cast<chrono::nanoseconds>(delta_duration).count();

  m_delta_time = delta_time_us * std::nano::num / std::nano::den;

  m_prev_frame_time = current_frame_time;
}

void Application::CreateMsFramebuffer() {
  gl::MsTextureCreateInfo texture_create_info;
  texture_create_info.samples = 4;
  texture_create_info.internal_format = GL_RGB;
  texture_create_info.fixed_sample_locations = true;

  m_ms_color_texture.Create(s_framebuffer_size, texture_create_info);

  m_ms_framebuffer.Create();
  m_ms_framebuffer.AttachMsTexture(m_ms_color_texture, GL_COLOR_ATTACHMENT0);

  m_ms_framebuffer.Bind();

  auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (result != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("cant complete ms framebuffer");
  }
}

void Application::CreatePostFramebuffer() {
  gl::TextureCreateInfo texture_create_info;
  texture_create_info.format = GL_RGB;
  texture_create_info.internal_format = GL_RGB;
  texture_create_info.mag_filter = GL_LINEAR;
  texture_create_info.min_filter = GL_LINEAR;
  texture_create_info.wrap_s = GL_CLAMP_TO_EDGE;
  texture_create_info.wrap_t = GL_CLAMP_TO_EDGE;
  texture_create_info.type = GL_UNSIGNED_BYTE;

  m_post_color_texture.Create(s_framebuffer_size, texture_create_info);
  m_post_framebuffer.Create();
  m_post_framebuffer.AttachTexture(m_post_color_texture, GL_COLOR_ATTACHMENT0);

  m_post_framebuffer.Bind();

  auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (result != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("cant complete post framebuffer");
  }
}

void Application::SetViewport(glm::uvec2 size) {
  glViewport(0, 0, size.x, size.y);
}
