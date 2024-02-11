#pragma once
#include "gl/framebuffer.hpp"
#include "gl/program.hpp"
#include "gl/texture.hpp"
#include "gl/vao.hpp"
#include "gl/vbo.hpp"
#include "image-renderer.hpp"
#include "render-area-layout.hpp"
#include "x-root-window.hpp"

class Application {
public:
  Application();
  Application(Application &) = delete;
  Application &operator=(Application &) = delete;

  int Run();

private:
  void Init();
  void MainLoop();
  void Cleanup();

  void DrawToFrambuffer();
  void DrawToScreen();

  void CreateFramebuffer();

  void SetViewport(glm::uvec2 size);

private:
  XRootWindow m_window;

  gl::Framebuffer m_framebuffer;
  gl::Texture m_color_texture;

  gl::Texture m_image_texture;

  ImageRenderer m_image_renderer;
  RenderAreaLayout m_render_area;

  static const char *const s_test_media_path;
};
