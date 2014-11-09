layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 4) in vec3 weights;
layout(location = 5) in ivec3 bone_index;
layout(location = 6) in vec2 texture_coord;


#include uniform_buffers.glsl
#include skinning.glsl

out vec3 normal_eye;
out vec2 st;


void main(void) 
{
  mat4 m = model;
  if (animated.x == 1.0) {
    m = model * func_animation_matrix_get();
  }
  
  mat4 model_view = view * m;
  mat3 normal_matrix = mat3(transpose(inverse(model_view)));

  vec3 position_eye = (model_view * vec4(vertex_position, 1.0)).xyz;

  // Out variables
  normal_eye = normal_matrix * vertex_normal;
  gl_Position = proj * vec4(position_eye, 1.0);
  st = texture_coord;
}
