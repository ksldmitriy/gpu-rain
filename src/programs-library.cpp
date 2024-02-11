#include "programs-library.hpp"

#include "shader-resources.hpp"

void ProgramsLibrary::CreatePrograms() {
  texture_program = gl::Program::CreateProgram(
      texture_vert_shader_resource, texture_vert_shader_resource_size,
      texture_frag_shader_resource, texture_frag_shader_resource_size);
}

gl::Program &ProgramsLibrary::GetTextureProgram() {
  return texture_program;
}

gl::Program ProgramsLibrary::texture_program;
