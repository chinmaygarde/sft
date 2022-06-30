#version 450core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec4 color;

layout(location = 1) out vec4 v_color;

void main() {
  v_color = color;
  gl_Position = vec4(vertex_position, 1.0);
}
