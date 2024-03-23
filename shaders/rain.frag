#version 450
in vec2 f_uv;
in vec2 f_screen_uv;
in float f_real_depth;
flat in int f_is_falling;

out vec4 FragColor;

layout(binding = 1) uniform sampler2D depth_texture;
layout(binding = 2) uniform sampler2D drop_fall_texture;
layout(binding = 3) uniform sampler2D drop_splash_texture;

void main() {
  float bg_depth = texture(depth_texture, f_screen_uv).x;

  if (f_real_depth > bg_depth) {
    discard;
  }

  if (f_is_falling != 0) {
    vec4 tex_sample = texture(drop_fall_texture, f_uv);
    FragColor = tex_sample;
  } else {
    vec4 tex_sample = texture(drop_splash_texture, f_uv);
    FragColor = tex_sample;
  }
}
