#version 450
layout(location = 0) in vec2 a_fall_pos;
layout(location = 1) in vec2 a_splash_pos;
layout(location = 2) in vec2 a_uv;

struct Droplet {
  vec3 pos;
  float splash_time;
  vec2 top_down_uv;
};

layout(std430, binding = 0) readonly restrict buffer DropletsSSBO {
  Droplet droplets_arr[];
};

out vec2 f_uv;
out vec2 f_screen_uv;
out float f_real_depth;
flat out int f_is_falling;

uniform mat4 u_transform_matrix;
uniform float u_near_plane;
uniform float u_far_plane;
uniform float u_min_drop_size;
uniform float u_max_drop_size;

uniform sampler2D u_depth_texture;

float map(float value, vec2 from, vec2 to);

void main() {
  uint index = gl_InstanceID;
  Droplet drop = droplets_arr[index];

  vec4 pos4 = u_transform_matrix * vec4(drop.pos, 1);
  vec3 pos = vec3(pos4) / pos4.w;

  vec2 vertex;

  if (drop.splash_time == 0) {
    f_is_falling = 1;
    vertex = a_fall_pos;
  } else {
    f_is_falling = 0;
    vertex = a_splash_pos;
  }

  vec2 scaled_vertex = vertex / 2.3f;
  // vec2 scaled_vertex = vertex * map(pos4.w, vec2(u_near_plane, u_far_plane),
  //                                   vec2(u_max_drop_size, u_min_drop_size));

  vec2 v_pos = vec2(pos) + scaled_vertex;

  f_screen_uv = (vec2(v_pos) + vec2(1, 1)) / 2.f;
  f_screen_uv.y = 1.f - f_screen_uv.y;
  f_real_depth = pos4.w;

  f_uv = a_uv;

  // gl_Position = vec4(v_pos, (pos.z + 1.f) / 2.f, 1);
  gl_Position = vec4(v_pos, 0, 1);
}

float map(float value, vec2 from, vec2 to) {
  return to.x + (value - from.x) * (to.y - to.x) / (from.y - from.x);
}
