#include uniform_buffers.glsl

layout (location = 0) out float fragment_depth;
//out vec4 def_shadow;

void main () 
{
 // def_shadow.r = 1;
 // def_shadow.g = 0;
 // def_shadow.b = 0;
 // def_shadow.a = 1;
  fragment_depth = gl_FragCoord.z;
}

