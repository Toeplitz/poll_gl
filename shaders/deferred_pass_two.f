#version 330


in vec2 st;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D depth_tex;

layout(std140) uniform GlobalMatrices {
  mat4 proj;
  mat4 inv_proj;
  mat4 view;
};

out vec4 frag_color;

vec3 func_positon_from_depth()
{
  float depth = texture2D(depth_tex, st).x;
  depth = (depth * 2.0) - 1.0;
  vec2 ndc = (st * 2.0) - 1.0;
  vec4 pos = vec4(ndc, depth, 1.0);
  pos = inv_proj * pos;
  return vec3(pos.xyz / pos.w);
}

void main () 
{
  vec4 p_texel = texture2D(position_tex, st);
  vec4 n_texel = texture2D(normal_tex, st);
  vec4 d_texel = texture2D(depth_tex, st);

//  frag_color.rgb =  n_texel.rgb;
//  frag_color.rgb =  d_texel.rgb;
//    frag_color.rgb =  p_texel.rgb;
  frag_color.rgb =  func_positon_from_depth();
  //frag_color.rgb = normalize(n_texel.rgb);
  frag_color.a = 1.0;
}

