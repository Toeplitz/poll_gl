
layout(location = 0) in vec3 vertex_position;


void main()
{
 // st = (vec2(vertex_position) + 1.0) * 0.5;
  gl_Position = vec4(vertex_position, 1.0);
}


