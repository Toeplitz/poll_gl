layout(location = 0) in vec3 vertex_position;

#include uniform_buffers.glsl


void main(void) 
{
  gl_Position = proj * view * model * vec4(vertex_position, 1.0);
}
