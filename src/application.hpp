#pragma once
#include "gl/framebuffer.hpp"
#include "gl/program.hpp"
#include "gl/texture.hpp"
#include "gl/vao.hpp"
#include "gl/vbo.hpp"
#include "image-renderer.hpp"
#include "rain-simulation.hpp"
#include "render-area-layout.hpp"
#include "x-root-window.hpp"

#include <chrono>

namespace chrono = std::chrono;
using hires_clock = chrono::high_resolution_clock;

typedef hires_clock::time_point time_point_t;
typedef hires_clock::duration duration_t;

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

  void UpdateDeltaTime();

  void CreateFramebuffer();

  void SetViewport(glm::uvec2 size);

private:
  XRootWindow m_window;

  gl::Framebuffer m_framebuffer;
  gl::Texture m_color_texture;
  gl::Texture m_depth_attachment;

  gl::Texture m_image_texture;

  ImageRenderer m_image_renderer;
  RenderAreaLayout m_render_area;

  RainSimulation m_rain_simulation;

  time_point_t m_prev_frame_time;
  float m_delta_time;

  static const size_t s_drops_count;
  
  static const glm::fvec2 s_drop_size;
  static const glm::fvec2 s_splash_size;

  static const char *const s_test_media_path;
  static const glm::uvec2 s_framebuffer_size;
};
