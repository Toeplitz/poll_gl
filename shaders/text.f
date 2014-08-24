#version 330

in vec2 st;

uniform sampler2D bitmap_texture;

out vec4 frag_color;

void main()
{
  float texel = texture(bitmap_texture, st).r;
  frag_color.rgb = vec3(1, 1, 1);
  frag_color.a = texel; 
}



