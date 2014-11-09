
#include uniform_buffers.glsl
#include ssao.glsl
#include light.glsl


uniform sampler2D normal_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D depth_tex;
uniform sampler2DShadow shadow_tex;
//uniform sampler2D shadow_tex;
uniform sampler3D offset_tex;
vec3 offset_tex_size = vec3(4, 8, (4 * 8) /2);
float radius = .03;

layout (location = 0) out vec4 out_color;
//out vec4 frag_color;

uniform mat4 shadow_view_projection;

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


float shadow_opengl_tut_get(vec4 shadow_coord)
{
  float bias = 0.005;

  float shadow = 1.0;
  for (int i = 0; i < 4; i++) {
		int index = i;
		//int index = int(16.0*random(gl_FragCoord.xyy, i))%16;
		//int index = int(16.0*random(floor(p_texel.xyz*1000.0), i))%16;

		shadow -= 0.2* (1.0 - texture(shadow_tex, vec3(shadow_coord.xy + poisson_disk[index] / 700.0,
    (shadow_coord.z-bias)/shadow_coord.w) ));
  }

  return shadow;
}

float shadow_cookbook_get(vec4 shadow_coord)
{
  float shadow = textureProj(shadow_tex, shadow_coord);

  return shadow;
}


float shadow_cookbook_pcf_get(vec4 shadow_coord)
{
  float sum = 0;

  sum += textureProjOffset(shadow_tex, shadow_coord, ivec2(-1, -1));
  sum += textureProjOffset(shadow_tex, shadow_coord, ivec2(-1, 1));
  sum += textureProjOffset(shadow_tex, shadow_coord, ivec2(1, 1));
  sum += textureProjOffset(shadow_tex, shadow_coord, ivec2(1, -1));

  return sum * 0.25;
}


float shadow_cookbook_pcf_soft_get(vec4 shadow_coord)
{
  ivec3 offset_coord;
  offset_coord.xy = ivec2(mod(gl_FragCoord.xy, offset_tex_size.xy));

  float sum = 0.0;
  int samples_div2 = int(offset_tex_size.z);
  vec4 sc = shadow_coord;

  for (int i = 0; i < 4; i++) {
    offset_coord.z = i;
    vec4 offsets = texelFetch(offset_tex, offset_coord, 0) * radius * shadow_coord.w;
    sc.xy = shadow_coord.xy + offsets.xy;
    sum += textureProj(shadow_tex, sc);
    sc.xy = shadow_coord.xy + offsets.zw;
    sum += textureProj(shadow_tex, sc);
  }

  float shadow = sum / 8.0;
  if (shadow != 1.0 && shadow != 0.0) {
    for (int i = 4; i < samples_div2; i++) {
      offset_coord.z = i;
      vec4 offsets = texelFetch(offset_tex, offset_coord, 0) * radius * shadow_coord.w;
      sc.xy = shadow_coord.xy + offsets.xy;
      sum += textureProj(shadow_tex, sc);
      sc.xy = shadow_coord.xy + offsets.zw;
      sum += textureProj(shadow_tex, sc);
    }
    shadow = sum / float(samples_div2 * 2.0);
  }

  return shadow;
}


vec4 shadow_coord_get(mat4 vp, vec3 position)
{
  vec4 shadow_coord = vp * vec4(position, 1.0);
  shadow_coord.xyz /= shadow_coord.w;
  shadow_coord.xyz += 1.0;
  shadow_coord.xyz *= 0.5;

  return shadow_coord;
}


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

  vec4 shadow_coord = shadow_coord_get(shadow_view_projection, p_texel);
  //float shadow = shadow_opengl_tut_get(shadow_coord);
  //float shadow = shadow_cookbook_get(shadow_coord);
  //float shadow = shadow_cookbook_pcf_get(shadow_coord);
  float shadow = shadow_cookbook_pcf_get(shadow_coord);

  /*
  float f = texture(shadow_tex, shadow_coord.xy).z;
  if (f < shadow_coord.z) {
    shadow = 0.5;
  }
  */

  out_color.rgb = light_apply(pos_eye, normalize(n_texel.rgb), vec3(diffuse_texel), shadow);
  //out_color.rgb = vec3(p_shadow_map, p_shadow_map, p_shadow_map);
  //out_color.rgb = vec3(diffuse_texel.a, diffuse_texel.a, diffuse_texel.a);
 // out_color.rgb = vec3(0, 1, 0);
  //frag_color.rgb = occlusion * phong(pos_eye, normalize(n_texel.rgb), vec3(0.5, 0.5, 0.5));
 // frag_color.rgb = vec3(n_texel);
  //frag_color.rgb = vec3(occlusion, occlusion, occlusion);

  out_color.a = 1.0;
}

