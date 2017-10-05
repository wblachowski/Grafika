#version 330 core

layout (location = 3) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 texcoords;

void main() {
  texcoords = aPos;
  gl_Position = projection * view *model* vec4(aPos, 1.0);
}