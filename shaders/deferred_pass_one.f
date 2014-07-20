#version 330

in vec3 position_eye;
in vec3 normal_eye;

layout (location = 0) out vec3 def_position;
layout (location = 1) out vec3 def_normal;

//out vec4 f;

void main () {
  //f = vec4 (depth,depth,depth, 1.0);
  def_position = position_eye;
  def_normal = normal_eye;
}

