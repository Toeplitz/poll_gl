
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

#include uniform_buffers.glsl

smooth out vec4 theColor;

void main()
{
  gl_Position = projection * view * position;
  theColor = color;
}
