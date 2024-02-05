#include "application.hpp"

#include "glad/gl.h"

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
}

void Application::MainLoop() {
  while (true) {
    glClearColor(0.12f, 0.12f, 0.12f, 0.12f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_window.SwapBuffers();
  }
}

void Application::Cleanup() {
  m_window.Close();
}
