#include "programs-library.hpp"

#include "gl/shader.hpp"
#include "shader-resources.hpp"

#include <glad/gl.h>
#include <stdexcept>

void ProgramsLibrary::CreatePrograms() {
  texture_program = gl::Program::CreateProgram(
      texture_vert_shader_resource, texture_vert_shader_resource_size,
      texture_frag_shader_resource, texture_frag_shader_resource_size);

  CreateRainComputeProgram();

  rain_draw_program = gl::Program::CreateProgram(
      rain_vert_shader_resource, rain_vert_shader_resource_size,
      rain_frag_shader_resource, rain_frag_shader_resource_size);
}

gl::Program &ProgramsLibrary::GetTextureProgram() {
  return texture_program;
}

gl::Program &ProgramsLibrary::GetRainComputeProgram() {
  return rain_compute_program;
}

gl::Program &ProgramsLibrary::GetRainDrawProgram() {
  return rain_draw_program;
}

void ProgramsLibrary::CreateRainComputeProgram() {
  gl::Shader shader;
  shader.Create(rain_comp_shader_resource, rain_comp_shader_resource_size,
                GL_COMPUTE_SHADER);

  if (!shader.IsSuccessfullyCompiled()) {
    throw std::runtime_error("rain compute shader error: " +
                             shader.GetCompileError());
  }

  rain_compute_program.Create();
  rain_compute_program.AttachShader(shader);
  rain_compute_program.Link();

  if (!rain_compute_program.IsSuccessfullyLinked()) {
    throw std::runtime_error("rain compute program link error: " +
                             rain_compute_program.GetLinkError());
  }

  shader.Delete();
}

gl::Program ProgramsLibrary::texture_program;
gl::Program ProgramsLibrary::rain_compute_program;
gl::Program ProgramsLibrary::rain_draw_program;
