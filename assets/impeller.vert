#version 450core

uniform FrameInfo {
  mat4 mvp;
}
frame_info;

in vec2 vertex_position;
out vec2 v_screen_position;

void main() {
  v_screen_position = vertex_position;
  gl_Position = frame_info.mvp * vec4(vertex_position, 0.0, 1.0);
}
