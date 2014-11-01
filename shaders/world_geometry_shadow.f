#include uniform_buffers.glsl

layout (location = 0) out vec3 def_normal;
layout (location = 1) out vec4 def_diffuse;

void main () 
{
  def_diffuse.a = gl_FragCoord.z;
  def_diffuse.a = 0.5;
}

