#pragma once
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
  
private:
  XRootWindow m_window;
};
