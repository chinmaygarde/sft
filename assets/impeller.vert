#version 450core

layout(binding = 0) uniform FrameInfo {
  mat4 mvp;
}
frame_info;

layout(location = 0) in vec2 vertex_position;
layout(location = 1) out vec2 v_screen_position;

void main() {
  v_screen_position = vertex_position;
  gl_Position = frame_info.mvp * vec4(vertex_position, 0.0, 1.0);
}
