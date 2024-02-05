#include "main.hpp"
#include "application.hpp"

#include <iostream>
#include <stdexcept>

#include "X11/Xlib.h"

int main() {
  Application app;

  int exit_code;

  try {
    exit_code = app.Run();
  } catch (std::runtime_error &e) {
    std::cout << "application finished with error: " << e.what() << std::endl;
  }

  return exit_code;
}
