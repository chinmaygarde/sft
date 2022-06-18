#version 450core

uniform FragInfo {
  vec2 texture_size;
  float time;
}
frag_info;

in vec2 v_screen_position;
out vec4 frag_color;

void main() {
  frag_color = vec4(v_screen_position, 0.0, 1.0);
}
