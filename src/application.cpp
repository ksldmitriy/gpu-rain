#include "application.hpp"

#include "gl/shader.hpp"
#include "programs-library.hpp"
#include "render-area-layout-constructor.hpp"
#include "shader-resources.hpp"

#include <glad/gl.h>
#include <iostream>
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

  CreateFramebuffer();

  ProgramsLibrary::CreatePrograms();

  std::vector<MonitorDimensions> monitor_dimentions =
      m_window.GetMonitorDimensions();

  RenderAreaLayoutConstructor render_area_constructor;
  render_area_constructor.ConstructLayout(monitor_dimentions,
                                          s_framebuffer_size,
                                          m_window.GetSize(), m_render_area);

  m_image_renderer.Create(s_test_media_path);

  float drop_size = 9;
  m_rain_simulation.Create(1024 * 2.f, s_framebuffer_size, drop_size);

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
  m_framebuffer.Bind();
  SetViewport(s_framebuffer_size);

  glClearColor(1.0f, 0.0f, 0.0f, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDisable(GL_DEPTH_TEST);
  m_image_renderer.Draw();

  // glEnable(GL_DEPTH_TEST);
  //  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  m_rain_simulation.Draw();
}

void Application::DrawToScreen() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  SetViewport(m_window.GetSize());

  glClearColor(0.0f, 0.0f, 1.0f, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);

  ProgramsLibrary::GetTextureProgram().Use();
  m_render_area.vao.Bind();
  m_render_area.ebo.Bind();

  m_color_texture.Bind();

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

void Application::CreateFramebuffer() {
  gl::TextureCreateInfo color_create_info;
  color_create_info.format = GL_RGB;
  color_create_info.internal_format = GL_RGB;
  color_create_info.min_filter = GL_LINEAR;
  color_create_info.mag_filter = GL_LINEAR;
  color_create_info.wrap_s = GL_CLAMP_TO_EDGE;
  color_create_info.wrap_t = GL_CLAMP_TO_EDGE;
  color_create_info.type = GL_UNSIGNED_BYTE;

  m_color_texture.Create(s_framebuffer_size, color_create_info, nullptr);

  gl::TextureCreateInfo depth_create_info;
  depth_create_info.format = GL_DEPTH_COMPONENT;
  depth_create_info.internal_format = GL_DEPTH_COMPONENT32F;
  depth_create_info.min_filter = GL_LINEAR;
  depth_create_info.mag_filter = GL_LINEAR;
  depth_create_info.wrap_s = GL_CLAMP_TO_EDGE;
  depth_create_info.wrap_t = GL_CLAMP_TO_EDGE;
  depth_create_info.type = GL_FLOAT;

  m_depth_attachment.Create(s_framebuffer_size, depth_create_info, nullptr);

  m_framebuffer.Create();
  m_framebuffer.AttachTexture(m_color_texture, GL_COLOR_ATTACHMENT0);
  m_framebuffer.AttachTexture(m_depth_attachment, GL_DEPTH_ATTACHMENT);

  m_framebuffer.Bind();

  auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (result != GL_FRAMEBUFFER_COMPLETE) {
    switch (result) {
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      std::cout << "incompelete attach" << std::endl;
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
      std::cout << "unsup" << std::endl;
      break;
    }
  }
}

void Application::SetViewport(glm::uvec2 size) {
  glViewport(0, 0, size.x, size.y);
}

const char *const Application::s_test_media_path = "media/main-color.png";

const glm::uvec2 Application::s_framebuffer_size = glm::uvec2(1920, 1080);
