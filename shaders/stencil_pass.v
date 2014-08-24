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



void main(void) 
{
  gl_Position = proj * view * vec4(vertex_position, 1.0);
}
