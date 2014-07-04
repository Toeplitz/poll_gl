#version 330

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec4 vertex_normal;
layout(location = 2) in vec4 vtangent;
layout(location = 3) in vec4 bitangent;
layout(location = 4) in vec4 weights;
layout(location = 5) in ivec4 bone_index;
layout(location = 6) in vec2 texture_coord;

layout(std140) uniform GlobalMatrices {
  mat4 proj;
  mat4 view;
};

layout(std140) uniform Matrices {
  mat4 model;
};

layout(std140) uniform Armature {
  mat4 skinning[64];
};

layout(std140) uniform Material {
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
  float shininess;
};

layout(std140) uniform State {
  int state_animated;
  int state_debug;
  int state_diffuse;
  int state_diffuse_normal;
  int state_diffuse_specular_normal;
  int state_standard;
};


out vec2 st;
out vec3 view_dir_tan;
out vec3 light_dir_tan;
out vec3 position_eye;
out vec3 normal_eye;


mat4 animation_matrix_get()
{
  mat4 animation = 
    weights[0] * skinning[int(bone_index[0])] +
    weights[1] * skinning[int(bone_index[1])] +
    weights[2] * skinning[int(bone_index[2])];

  if (weights[0] == 0.0 && weights[1] == 0.0 && weights[2] == 0.0) {
    animation = model;
  }

  return animation;
}


void main(void) 
{
  mat4 m = model;

  if (state_animated == 1) {
    m = animation_matrix_get();
  }
  mat4 model_view = view * m;
  mat3 normal_matrix = mat3(transpose(inverse(model_view)));

  position_eye = vec3(model_view * vertex_position);
 // normal_eye = vec3(model_view * vec4(vec3(vertex_normal), 0));
  normal_eye = normalize(vec3(normal_matrix * vec3(vertex_normal)));
	//gl_Position = proj * model_view * vertex_position;
  gl_Position = proj * vec4(position_eye, 1.0);
	st = texture_coord;


}
