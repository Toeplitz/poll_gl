
layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_color;

#include uniform_buffers.glsl

smooth out vec4 out_color;

void main()
{
  gl_Position = view_projection * model * vertex_position;
  out_color = vertex_color;
}
