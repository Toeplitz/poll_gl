
#include uniform_buffers.glsl
#include light.glsl
#include ssao.glsl

uniform sampler2D normal_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D depth_tex;

layout (location = 0) out vec4 out_color;


void main () 
{
  vec2 st;
  st.s = gl_FragCoord.x / config_viewport.width;
  st.t = gl_FragCoord.y / config_viewport.height;

  vec4 n_texel = texture2D(normal_tex, st);
  vec4 diffuse_texel = texture2D(diffuse_tex, st);
  float d_texel = texture2D(depth_tex, st).r;

  vec3 p_texel = reconstruct_position(d_texel, st);
  vec3 pos_eye = vec3(view * vec4(p_texel.rgb, 1.0));

  float occlusion = ssoa(st, d_texel, pos_eye, n_texel.rgb, depth_tex);

  out_color.rgb = occlusion * light_apply(pos_eye, normalize(n_texel.rgb), vec3(diffuse_texel));
  out_color.a = 1.0;
}

