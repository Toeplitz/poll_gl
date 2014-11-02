
#include uniform_buffers.glsl
#include ssao.glsl
#include light.glsl


uniform sampler2D normal_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D depth_tex;
uniform sampler2DShadow shadow_tex;
//uniform sampler2D shadow_tex;


layout (location = 0) out vec4 out_color;
//out vec4 frag_color;

uniform mat4 shadow_bias;

vec2 poisson_disk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);


vec2 poisson_disk_16samples[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);


float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

/*
float chebyshevUpperBound(vec2 ShadowCoordPostW, float distance)
{
  vec2 moments = texture2D(shadow_tex, ShadowCoordPostW).rg;
  
  if (distance <= moments.x)
    return 1.0 ;

  float variance = moments.y - (moments.x*moments.x);
  variance = max(variance,0.00002);

  float d = distance - moments.x;
  float p_max = variance / (variance + d*d);

  return p_max;
}
*/



void main () 
{

  vec2 st;
  st.s = gl_FragCoord.x / config_viewport.width;
  st.t = gl_FragCoord.y / config_viewport.height;

  vec4 n_texel = texture2D(normal_tex, st);
  vec4 diffuse_texel = texture2D(diffuse_tex, st);
  float d_texel = texture2D(depth_tex, st).r;

  vec3 p_texel = reconstruct_position(d_texel, st);
  vec3 pos_eye = vec3(view * vec4(p_texel, 1.0));

  float bias = 0.005;
  float visibility = 1.0;
  vec4 shadow_coord = shadow_bias * vec4(p_texel, 1.0);

/*
  for (int i = 0; i < 4; i++) {
		int index = i;

		visibility -= 0.2* (1.0 - texture(shadow_tex, vec3(shadow_coord.xy + poisson_disk_16samples[index] / 700.0,
    (shadow_coord.z-bias)/shadow_coord.w) ));
  }
*/

  /*
  float p_shadow_map = texture(shadow_tex, shadow_coord.xy).r;
  if (p_shadow_map < shadow_coord.z) {
      visibility = 0.5;
   }
   */

 // vec4 ShadowCoordPostW = shadow_coord / shadow_coord.w;
//  float shadow = chebyshevUpperBound(ShadowCoordPostW.xy, ShadowCoordPostW.z);
 // visibility = shadow;

  visibility = textureProj(shadow_tex, shadow_coord / shadow_coord.w);

  out_color.rgb = light_apply(pos_eye, normalize(n_texel.rgb), vec3(diffuse_texel), visibility);
  //out_color.rgb = vec3(p_shadow_map, p_shadow_map, p_shadow_map);
  //out_color.rgb = vec3(diffuse_texel.a, diffuse_texel.a, diffuse_texel.a);
 // out_color.rgb = vec3(0, 1, 0);
  //frag_color.rgb = occlusion * phong(pos_eye, normalize(n_texel.rgb), vec3(0.5, 0.5, 0.5));
 // frag_color.rgb = vec3(n_texel);
  //frag_color.rgb = vec3(occlusion, occlusion, occlusion);

  out_color.a = 1.0;
}

