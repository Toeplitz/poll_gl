
layout(location = 0) in vec3 vertex_position;
layout(location = 4) in vec3 weights;
layout(location = 5) in ivec3 bone_index;


#include uniform_buffers.glsl
#include skinning.glsl

uniform mat4 shadow_view_projection;

void main(void) 
{
  mat4 m = model;
  if (state_animated == 1) {
    m = model * func_animation_matrix_get();
  }

  // Out variables
  gl_Position = shadow_view_projection * m * vec4(vertex_position, 1.0);
}
