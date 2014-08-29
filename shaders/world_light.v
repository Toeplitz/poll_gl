
layout(location = 0) in vec3 vertex_position;

#include uniform_buffers.glsl

void main()
{
  gl_Position = view_projection * model *  vec4(vertex_position, 1.0);
}


