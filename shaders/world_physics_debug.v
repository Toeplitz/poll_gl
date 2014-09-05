
layout (location = 0) in vec3 vertex_position;
layout (location = 0) in vec3 vertex_color;

#include uniform_buffers.glsl

flat out vec3 color;

void main()
{
  color = vertex_color;
  gl_Position = view_projection * vec4(vertex_position, 1.0);
}
