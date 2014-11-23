#include uniform_buffers.glsl
#include fxaa.glsl

in vec2 st;
out vec4 frag_color;
uniform sampler2D tex;
uniform sampler2D shadow_tex;


float test(float c)
{
  float x = max(0, c - 0.004);
  return (x * (6.2 * x + 0.5) / (x * 6.2 * x + 1.7) + 0.06);
}

vec3 test2(vec3 x)
{
  float A = 0.15;
  float B = 0.50;
  float C = 0.10;
  float D = 0.20;
  float E = 0.02;
  float F = 0.30;

  return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main()
{
  //frag_color = fxaa(st, tex);
  vec3 frag = texture(tex, st).rgb;

  //frag_color.rgb = vec3(test(frag.x), test(frag.y), test(frag.z));
  //frag_color.rgb = test2(frag);

  //vec3 color =  frag / (frag + 1.0);
  frag_color.rgb = frag;
  frag_color.rgb = vec3(1, 0, 0);

  //frag_color.rgb = frag;

  //vec3 shadow = texture(shadow_tex, st).rgb;
 // frag_color.rgb = shadow;
 /*
  float shadow = texture(shadow_tex, st).x;
  shadow = 1.0 - (1.0 - shadow) * 2;
  frag_color.rgb = vec3(shadow, shadow, shadow);
  */
  frag_color.a = 1.0; 
}



