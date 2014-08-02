#version 330

layout(location = 0) in vec3 vertex_position;

layout(std140) uniform GlobalMatrices {
  mat4 proj;
  mat4 inv_proj;
  mat4 view;
};


layout(std140) uniform Matrices {
  mat4 model;
};


layout(std140) uniform Light 
{
  vec4 light_position;
  mat4 light_transform;
};


//out vec2 st;

void main()
{
 // st = (vec2(vertex_position) + 1.0) * 0.5;
  gl_Position = proj * view * light_transform * vec4(vertex_position, 1.0);
 // gl_Position = vec4(vertex_position, 1.0);
}


