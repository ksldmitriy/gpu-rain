#include "x-root-window.hpp"

#include <KHR/khrplatform.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <glad/gl.h>
#include <glad/glx.h>
#include <stdexcept>

XRootWindow::XRootWindow() {
}

XRootWindow::~XRootWindow() {
  if (m_display) {
    CloseXDisplay();
  }
}

void XRootWindow::Open() {
  OpenXDisplay();

  m_screen = (_XScreen *)XScreenOfDisplay(m_display, screen_number);

  m_window = XRootWindowOfScreen((Screen *)m_screen);
  if (!m_window) {
    throw std::runtime_error("failed to open xroot window");
  }
}

void XRootWindow::Close() {
  CloseXDisplay();
}

void XRootWindow::CreateGLContext() {
  int glx_version = gladLoaderLoadGLX(m_display, screen_number);
  if (!glx_version) {
    throw std::runtime_error("cant load glx");
    return;
  }

  GLint visual_attributes[] = {GLX_RGBA,
                               GLX_DOUBLEBUFFER,
                               GLX_RED_SIZE,
                               8,
                               GLX_GREEN_SIZE,
                               8,
                               GLX_BLUE_SIZE,
                               8,
                               0};
  XVisualInfo *visual =
      glXChooseVisual(m_display, screen_number, visual_attributes);
  if (!visual) {
    throw std::runtime_error("xlib visula not found");
  }

  GLXContext context = glXCreateContext(m_display, visual, nullptr, true);
  if (!context) {
    throw std::runtime_error("cant to create opengl context");
    return;
  }

  glXMakeCurrent(m_display, m_window, context);

  int gl_version = gladLoaderLoadGL();
  if (!gl_version) {
    throw std::runtime_error("cant to load gl");
    return;
  }

  XWindowAttributes gwa;
  XGetWindowAttributes(m_display, m_window, &gwa);

  glViewport(0, 0, gwa.width, gwa.height);
}

void XRootWindow::SwapBuffers() {
  glXSwapBuffers(m_display, m_window);
}

std::vector<MonitorDimensions> XRootWindow::GetMonitorDimensions() {
  int monitors_count;
  XRRMonitorInfo *monitors_info =
      XRRGetMonitors(m_display, m_window, 1, &monitors_count);

  std::vector<MonitorDimensions> monitor_dimensions(monitors_count);

  for (int i = 0; i < monitors_count; i++) {
    MonitorDimensions dimensions;
    dimensions.position = glm::ivec2(monitors_info[i].x, monitors_info[i].y);
    dimensions.size =
        glm::uvec2(monitors_info[i].width, monitors_info[i].height);
  }

  XRRFreeMonitors(monitors_info);

  return monitor_dimensions;
}

void XRootWindow::OpenXDisplay() {
  m_display = XOpenDisplay(nullptr);
  if (m_display == nullptr) {
    throw std::runtime_error("cant open xdisplay");
  }
}

void XRootWindow::CloseXDisplay() {
  if (!m_display) {
    throw std::runtime_error("trying to close closed xdisplay");
  }

  XCloseDisplay(m_display);
}
