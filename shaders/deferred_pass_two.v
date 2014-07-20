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

uniform vec3 camera_position_world;

out vec2 st;

void main(void) 
{
  st = (vec2(vertex_position) + 1.0) * 0.5;
 // gl_Position = proj * view * model * vec4(vertex_position, 1.0);
  gl_Position = vec4(vertex_position, 1.0);
}
