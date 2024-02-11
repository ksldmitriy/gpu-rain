#pragma once
#include "gl/program.hpp"

class ProgramsLibrary {
public:
  ProgramsLibrary() = delete;

  static void CreatePrograms();

  static gl::Program &GetTextureProgram();

private:
  static gl::Program texture_program;
};
