#include uniform_buffers.glsl

layout (location = 0) out vec3 def_normal;
layout (location = 1) out vec4 def_diffuse;

in vec3 normal_eye;
in vec2 st;
uniform sampler2D diffuse_texture;

void main () 
{
  vec3 diffuse_out = texture(diffuse_texture, st).rgb;
  def_normal = normal_eye;
 // def_diffuse.rgb =  diffuse_out;
  def_diffuse.rgb = material_kd.rgb;
  //def_diffuse.rgb = vec3(0, 1, 0);
  //def_diffuse.a = 1;
}

