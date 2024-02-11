#include "application.hpp"

#include "gl/shader.hpp"
#include "programs-library.hpp"
#include "render-area-layout-constructor.hpp"
#include "shader-resources.hpp"

#include <glad/gl.h>
#include <iostream>
#include <stb/stb_image.h>

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
                                          m_color_texture.GetSize(),
                                          m_window.GetSize(), m_render_area);

  m_image_renderer.Create(s_test_media_path);
}

void Application::MainLoop() {
  while (true) {
    DrawToFrambuffer();
    DrawToScreen();
    m_window.SwapBuffers();
  }
}

void Application::DrawToFrambuffer() {
  m_framebuffer.Bind();
  SetViewport(m_color_texture.GetSize());

  glClearColor(1.0f, 0.0f, 0.0f, 0.12f);
  glClear(GL_COLOR_BUFFER_BIT);

  m_image_renderer.Draw();
}

void Application::DrawToScreen() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  SetViewport(m_window.GetSize());

  glClearColor(1.0f, 0.0f, 0.0f, 0.12f);
  glClear(GL_COLOR_BUFFER_BIT);

  ProgramsLibrary::GetTextureProgram().Use();
  m_render_area.vao.Bind();
  m_render_area.ebo.Bind();

  m_color_texture.Bind();

  glDrawElements(GL_TRIANGLES, m_render_area.index_count, GL_UNSIGNED_INT, 0);
}

void Application::Cleanup() {
  m_window.Close();
}

void Application::CreateFramebuffer() {
  gl::TextureCreateInfo create_info;
  create_info.format = GL_RGB;
  create_info.internal_format = GL_RGB;
  create_info.min_filter = GL_LINEAR;
  create_info.mag_filter = GL_LINEAR;
  create_info.wrap_s = GL_CLAMP_TO_EDGE;
  create_info.wrap_t = GL_CLAMP_TO_EDGE;

  m_color_texture.Create(glm::uvec2(1920, 1080), create_info, nullptr);

  m_color_texture.GetHandle();

  m_framebuffer.Create();
  m_framebuffer.AttachTexture(m_color_texture, GL_COLOR_ATTACHMENT0);
}

void Application::SetViewport(glm::uvec2 size) {
  glViewport(0, 0, size.x, size.y);
}

const char *const Application::s_test_media_path = "media/image.jpg";
