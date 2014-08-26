#version 330


layout(std140) uniform GlobalMatrices 
{
  mat4 proj;
  mat4 inv_proj;
  mat4 view;
};


layout(std140) uniform Matrices {
  mat4 model;
};


struct Config_Ssoa 
{
  float distance_threshold;
  float filter_radius;
  float sample_count;
};

struct Config_Viewport
{
  int width;
  int height;
};


layout(std140) uniform Config
{
  Config_Ssoa config_ssoa;
  Config_Viewport config_viewport;
};

layout(std140) uniform Light
{
  vec4 light_position;
  vec4 light_color;
  vec4 light_direction;
  int light_type;
};


const vec2 poisson16[] = vec2[] (
  // Poisson Disk Samples
  vec2( -0.94201624,  -0.39906216 ),
  vec2(  0.94558609,  -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2(  0.34495938,   0.29387760 ),
  vec2( -0.91588581,   0.45771432 ),
  vec2( -0.81544232,  -0.87912464 ),
  vec2( -0.38277543,   0.27676845 ),
  vec2(  0.97484398,   0.75648379 ),
  vec2(  0.44323325,  -0.97511554 ),
  vec2(  0.53742981,  -0.47373420 ),
  vec2( -0.26496911,  -0.41893023 ),
  vec2(  0.79197514,   0.19090188 ),
  vec2( -0.24188840,   0.99706507 ),
  vec2( -0.81409955,   0.91437590 ),
  vec2(  0.19984126,   0.78641367 ),
  vec2(  0.14383161,  -0.14100790 )
);


uniform sampler2D normal_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D depth_tex;

out vec4 frag_color;

vec3 d_Ls = vec3 (1.0, 1.0, 1.0); // white specular colour
vec3 d_Ld = vec3 (0.7, 0.7, 0.7); // dull white diffuse light colour
vec3 d_La = vec3 (0.2, 0.2, 0.2); // grey ambient colour

//vec3 kd = vec3 (0.9, 0.9, 0.9);
vec3 ks = vec3 (0.5, 0.5, 0.5);

float specular_exponent = 100.0;

vec3 phong(in vec3 op_eye, in vec3 n_eye, in vec3 kd) {
  vec3 lp_eye = (view * light_position).xyz;
  //vec3 lp_eye = (view * vec4(0, 20, 0, 1)).xyz;
  vec3 dist_to_light_eye = lp_eye - op_eye;
  vec3 direction_to_light_eye = normalize (dist_to_light_eye);

  // standard diffuse light
  float dot_prod = max(dot (direction_to_light_eye,  n_eye), 0.0);

  vec3 Id = vec3(light_color) * kd * dot_prod; // final diffuse intensity
  // standard specular light
  vec3 reflection_eye = reflect (-direction_to_light_eye, n_eye);
  vec3 surface_to_viewer_eye = normalize (-op_eye);
  float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
  dot_prod_specular = max (dot_prod_specular, 0.0);
  float specular_factor = pow (dot_prod_specular, specular_exponent);
  vec3 Is = vec3(light_color) * ks * specular_factor; // final specular intensity

  float dist_2d = max (0.0, 1.0 - distance (lp_eye, op_eye) / 30.0);
  float atten_factor =  dist_2d;

  //return vec3(dist_2d,dist_2d,dist_2d);

  //return (Id + Is) * atten_factor;
  return (Id + Is);

  //return Id;
}


vec3 reconstruct_position(float depth, vec2 tex_coord)
{
  vec4 position = vec4(tex_coord, depth, 1);
  position.xyz = position.xyz * 2 - 1;
  position = inverse(proj * view) * position;
  position.xyz /= position.w;
  return position.xyz;
}


float ssoa(vec2 st, float d_texel, vec3 pos_eye, vec3 n_texel)
{
  vec2 filterRadius = vec2(config_ssoa.filter_radius / config_viewport.width,
    config_ssoa.filter_radius / config_viewport.height);
  float ambientOcclusion = 0;

  for (int i = 0; i < config_ssoa.sample_count; ++i)
  {
      // sample at an offset specified by the current Poisson-Disk sample and scale it by a radius (has to be in Texture-Space)
      vec2 sampleTexCoord = st + (poisson16[i] * (filterRadius));
      float sampleDepth = texture(depth_tex, sampleTexCoord).r;

      vec3 samplePos = vec3(view * vec4(reconstruct_position(sampleDepth, sampleTexCoord), 1.0));
      vec3 sampleDir = normalize(samplePos - pos_eye);

      // angle between SURFACE-NORMAL and SAMPLE-DIRECTION (vector from SURFACE-POSITION to SAMPLE-POSITION)
      float NdotS = max(dot(n_texel, sampleDir), 0);
      // distance between SURFACE-POSITION and SAMPLE-POSITION
      float VPdistSP = distance(pos_eye, samplePos);

      // a = distance function
      float a = 1.0 - smoothstep(config_ssoa.distance_threshold, config_ssoa.distance_threshold * 2, VPdistSP);
      // b = dot-Product
      float b = NdotS;

      ambientOcclusion += (a * b);
  }

  return 1 - ambientOcclusion / config_ssoa.sample_count;
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
  vec3 pos_eye = vec3(view * vec4(p_texel.rgb, 1.0));

  float occlusion = ssoa(st, d_texel, pos_eye, n_texel.rgb);

  frag_color.rgb = occlusion * phong(pos_eye, normalize(n_texel.rgb), vec3(diffuse_texel));
 // frag_color.rgb = vec3(0, 1, 0);
  //frag_color.rgb = occlusion * phong(pos_eye, normalize(n_texel.rgb), vec3(0.5, 0.5, 0.5));
 // frag_color.rgb = vec3(n_texel);
  //frag_color.rgb = vec3(occlusion, occlusion, occlusion);

  frag_color.a = 1.0;
}

