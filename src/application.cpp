#include "application.hpp"

#include <iostream>

Application::Application() {
}

int Application::Run() {
  std::cout << "mock application" << std::endl;

  return 0;
}

void Application::Init() {
  m_window.Open();
}

void Application::MainLoop() {
}

void Application::Cleanup() {
  m_window.Close();
}
