layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 4) in vec3 weights;
layout(location = 5) in ivec3 bone_index;
layout(location = 6) in vec2 texture_coord;


#include uniform_buffers.glsl
#include skinning.glsl

uniform mat4 shadow_view_projection;

void main(void) 
{
  mat4 m = model;
  if (animated.x == 1.0) {
    m = model * func_animation_matrix_get();
  }

  // Out variables
  gl_Position = shadow_view_projection * m * vec4(vertex_position, 1.0);
}
