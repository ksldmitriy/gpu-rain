#include "x-root-window.hpp"

#include <X11/Xlib.h>
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

  Screen *f;
  m_screen = (_XScreen *)XDefaultScreenOfDisplay(m_display);

  m_window = XRootWindowOfScreen((Screen *)m_screen);
  if (!m_window) {
    throw std::runtime_error("failed to open xroot window");
  }
}

void XRootWindow::Close() {
  CloseXDisplay();
}

void XRootWindow::OpenXDisplay() {
  m_display = XOpenDisplay("");
  if (!m_display) {
    throw std::runtime_error("cant open xdisplay");
  }
}

void XRootWindow::CloseXDisplay() {
  if (!m_display) {
    throw std::runtime_error("trying to close closed xdisplay");
  }

  XCloseDisplay(m_display);
}
