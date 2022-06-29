#version 450core

layout(location = 0) in vec3 vertex_position;

void main() {
  gl_Position = vec4(vertex_position + vec3(0.25, 0.25, 0), 1.0);
}
