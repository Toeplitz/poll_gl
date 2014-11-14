#include uniform_buffers.glsl

layout (location = 0) out vec3 def_normal;
layout (location = 1) out vec4 def_diffuse;

in vec3 normal_eye;
in vec2 st;
uniform sampler2D diffuse_texture;

void main () 
{
  vec3 diffuse_out = texture(diffuse_texture, st).rgb;
 // vec3 diffuse_out = vec3(1, 1, 1);
  def_normal = normal_eye;
  def_diffuse.rgb =  diffuse_out;
}

