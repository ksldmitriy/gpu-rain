#pragma once
#include "gl/program.hpp"

class ProgramsLibrary {
public:
  ProgramsLibrary() = delete;

  static void CreatePrograms();

  static gl::Program &GetTextureProgram();
  static gl::Program &GetRainComputeProgram();
  static gl::Program &GetRainDrawProgram();

private:
  static void CreateRainComputeProgram();
  
private:
  static gl::Program texture_program;
  static gl::Program rain_compute_program;
  static gl::Program rain_draw_program;
};
