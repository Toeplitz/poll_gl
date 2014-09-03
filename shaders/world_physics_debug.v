
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;

#include uniform_buffers.glsl

out vec3 out_color;

void main()
{
  gl_Position = view_projection * vec4(vertex_position, 1.0);
  out_color = vertex_color;
}
