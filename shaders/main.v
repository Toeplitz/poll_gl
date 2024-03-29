#version 330

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec3 vtangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec3 weights;
layout(location = 5) in ivec3 bone_index;
layout(location = 6) in vec2 texture_coord;

layout(std140) uniform GlobalMatrices {
  mat4 proj;
  mat4 inv_proj;
  mat4 view;
};

layout(std140) uniform Matrices {
  mat4 model;
};

layout(std140) uniform Armature {
  mat4 skinning[64];
};

layout(std140) uniform Material {
  vec4 Ka;
  vec4 Kd;
  vec4 Ks;
  float shininess;
};

layout(std140) uniform Node_State {
  int state_animated;
  int state_debug;
  int state_diffuse;
  int state_diffuse_normal;
  int state_diffuse_specular_normal;
  int state_cubemap_reflect;
  int state_cubemap_skybox;
  int state_standard;
};


out vec2 st;
out vec3 view_dir_tan;
out vec3 light_dir_tan;
out vec3 position_eye;
out vec3 normal_eye;
out mat4 model_world;
out vec3 position;
out vec3 normal;
out mat4 model_view;
out mat3 normal_matrix;

// Cubemap (skybox/reflection/refraction)
out vec3 str;


mat4 func_animation_matrix_get()
{
  mat4 animation = 
    weights[0] * skinning[int(bone_index[0])] +
    weights[1] * skinning[int(bone_index[1])] +
    weights[2] * skinning[int(bone_index[2])];

  //if (weights[0] == 0.0 && weights[1] == 0.0 && weights[2] == 0.0) {
  //  animation = model;
  //}

  return animation;
}


void main(void) 
{
  mat4 m = model;

  if (state_animated == 1) {
    m = model * func_animation_matrix_get();
  }
  model_view = view * m;
  normal_matrix = mat3(transpose(inverse(model_view)));
 

  // Out variables
  position = vertex_position;
  normal = vertex_normal;
  position_eye = vec3(model_view * vec4(vertex_position, 1.0));
  normal_eye = normalize(normal_matrix * vertex_normal);
	st = texture_coord;
  str = vertex_position;
  gl_Position = proj * vec4(position_eye, 1.0);
}
