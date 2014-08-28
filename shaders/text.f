
in vec2 st;
out vec4 frag_color;
uniform sampler2D bitmap_texture;

void main()
{
  float texel = texture(bitmap_texture, st).r;
  frag_color.rgb = vec3(1, 1, 1);
  frag_color.a = texel; 
}



