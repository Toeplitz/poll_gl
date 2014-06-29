#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

layout(std140) uniform GlobalMatrices {
  mat4 projection;
  mat4 view;
};

smooth out vec4 theColor;

void main()
{
  gl_Position = projection * view * position;
  theColor = color;
}
