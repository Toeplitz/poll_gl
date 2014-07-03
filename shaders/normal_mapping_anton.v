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


layout(std140) uniform Debug {
  vec4 debug;
};


out vec2 st;
out vec3 view_dir_tan;
out vec3 light_dir_tan;


void main(void) {
	gl_Position = proj * view * model * vertex_position;
	st = texture_coord;

	/* HACK */
	vec3 cam_pos_wor = (inverse (view) * vec4 (0.0, 0.0, 0.0, 1.0)).xyz;
	vec3 light_dir_wor = vec3 (0.0, 0.0, -1.0);
	
	/* work out bi-tangent as cross product of normal and tangent. also multiply
		 by the determinant, which we stored in .w to correct handedness
	*/ 
	vec3 bitangent = cross (vec3(vertex_normal), vtangent.xyz) * vtangent.w;
	
	/* transform our camera and light uniforms into local space */
	vec3 cam_pos_loc = vec3 (inverse (model) * vec4 (cam_pos_wor, 1.0));
	vec3 light_dir_loc = vec3 (inverse (model) * vec4 (light_dir_wor, 0.0));
	// ...and work out view _direction_ in local space
	vec3 view_dir_loc = normalize (cam_pos_loc - vec3(vertex_position));
	
	/* this [dot,dot,dot] is the same as making a 3x3 inverse tangent matrix, and
		 doing a matrix*vector multiplication.
	*/
	// work out view direction in _tangent space_
	view_dir_tan = vec3 (
		dot (vtangent.xyz, view_dir_loc),
		dot (bitangent, view_dir_loc),
		dot (vec3(vertex_normal), view_dir_loc)
	);
	// work out light direction in _tangent space_
	light_dir_tan = vec3 (
		dot (vtangent.xyz, light_dir_loc),
		dot (bitangent, light_dir_loc),
		dot (vec3(vertex_normal), light_dir_loc)
	);

}
