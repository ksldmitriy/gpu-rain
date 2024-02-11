#version 450
in vec2 f_uv;

out vec4 FragColor;

uniform sampler2D u_texture;

void main() {
  vec4 texture_sample = texture(u_texture, f_uv);

  FragColor = texture_sample;
}
