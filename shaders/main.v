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

//vec3 light_position_world  = vec3 (-10.0, 10.0, -4.0);
vec3 light_position_world  = vec3 (5, 22, 24.0);
vec3 light_dir_wor = vec3 (0.0, 0.0, -1.0);

mat4 animation_matrix_get()
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
    m = model * animation_matrix_get();
  }
  model_view = view * m;
  normal_matrix = mat3(transpose(inverse(model_view)));

  position = vertex_position;
  normal = vertex_normal;

  position_eye = vec3(model_view * vec4(vertex_position, 1.0));
  //normal_eye = normalize(normal_matrix * vertex_normal);
  normal_eye = (vec3(view * m * vec4 (vertex_normal, 0.0)));

 // normal_eye = vec3(model_view * vec4(vertex_normal, 0.0));

  gl_Position = proj * vec4(position_eye, 1.0);
	st = texture_coord;
  model_world = model;

  // Cubemap (skybox)
  str = vertex_position;

  if (state_diffuse_specular_normal == 1 || state_diffuse_normal == 1) {
    /* HACK */
    vec3 cam_pos_wor = (inverse(view) * vec4 (0.0, 0.0, 0.0, 1.0)).xyz;
    
    /* work out bi-tangent as cross product of normal and tangent. also multiply
       by the determinant, which we stored in .w to correct handedness
    */ 
    //vec3 bitangent = cross (vertex_normal, vtangent) * vtangent.w;
    //vec3 bitangent = cross (vertex_normal, vtangent) * 1;
    
    /* transform our camera and light uniforms into local space */
    vec3 cam_pos_loc = vec3(inverse(model)  * vec4 (cam_pos_wor, 1.0));
    vec3 light_dir_loc = vec3(inverse(model) * vec4 (light_dir_wor, 0.0));
    // ...and work out view _direction_ in local space
    vec3 view_dir_loc = normalize (cam_pos_loc - vertex_position);
    
    /* this [dot,dot,dot] is the same as making a 3x3 inverse tangent matrix, and
       doing a matrix*vector multiplication.
    */
    // work out view direction in _tangent space_
    view_dir_tan = vec3 (
      dot (vtangent, view_dir_loc),
      dot (bitangent, view_dir_loc),
      dot (vertex_normal, view_dir_loc)
    );
    // work out light direction in _tangent space_
    light_dir_tan = vec3 (
      dot (vtangent, light_dir_loc),
      dot (bitangent, light_dir_loc),
      dot (vertex_normal, light_dir_loc)
    );
  }

}
