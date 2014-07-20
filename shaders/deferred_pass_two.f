#version 330
#extension GL_ARB_shading_language_420pack: enable  

//layout(binding = 0) uniform sampler2D position_tex;
//layout(binding = 1) uniform sampler2D normal_tex;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;

in vec2 st;
out vec4 frag_color;

void main () 
{
  vec2 st_foo;
  st_foo.s = gl_FragCoord.x / 800.0;
  st_foo.t = gl_FragCoord.y / 800.0;

  vec4 p_texel = texture2D(position_tex, st);
  // skip background
  if (p_texel.z > -0.0001) {
    discard;
  }

  vec4 n_texel = texture2D(normal_tex, st);

  //frag_color.rgb = phong (p_texel.rgb, normalize (n_texel.rgb));
  frag_color.rgb = p_texel.rgb;
 // frag_color.rgb = normalize(n_texel.rgb);
//  frag_color.rgb = vec3(0, 1, 0);
  frag_color.a = 1.0;
}

