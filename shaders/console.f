#version 330

in vec2 st;

uniform sampler2D bitmap_texture;

out vec4 frag_color;

void main()
{
  vec3 out_color = vec3(0, 1, 0);
  float texel = texture(bitmap_texture, st).r;
  frag_color.rgb = out_color;
  frag_color.rgb = vec3(texel, texel, texel);
  
  frag_color.a = 0.5; 
}



