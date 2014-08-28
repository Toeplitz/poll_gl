layout (location = 0) out vec3 def_normal;
layout (location = 1) out vec3 def_diffuse;

in vec3 normal_eye;
in vec2 st;
uniform sampler2D diffuse_texture;

void main () 
{
  vec3 diffuse_out = texture(diffuse_texture, st).rgb;
  def_normal = normal_eye;
  def_diffuse =  diffuse_out;
}

