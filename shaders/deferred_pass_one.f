#version 330

in vec3 normal_eye;
in vec2 st;

uniform sampler2D diffuse_texture;
//uniform sampler2D normal_texture;
//uniform sampler2D specular_texture;
//uniform samplerCube cube_texture;

layout (location = 0) out vec3 def_normal;
layout (location = 1) out vec3 def_diffuse;

void main () 
{
  vec3 diffuse_out = texture(diffuse_texture, st).rgb;
  def_normal = normal_eye;
  def_diffuse =  diffuse_out;
}

