#include uniform_buffers.glsl
#include fxaa.glsl

in vec2 st;
out vec4 frag_color;
uniform sampler2D tex;

void main()
{
 // frag_color = fxaa(st, tex);
  frag_color.rgb = texture(tex, st).rgb;
  frag_color.a = 1.0; 
}



