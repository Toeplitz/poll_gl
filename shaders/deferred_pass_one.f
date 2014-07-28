#version 330

in vec3 normal_eye;
in vec2 st;

uniform sampler2D diffuse_texture;
uniform sampler2D normal_texture;
uniform sampler2D specular_texture;
uniform samplerCube cube_texture;

layout (location = 0) out vec3 def_normal;
layout (location = 1) out vec3 def_diffuse;

void main () 
{
  def_normal = normal_eye;
  def_diffuse = texture(diffuse_texture, st).rgb;
}

