#version 330
#define MAX_LIGHTS 8
#define LIGHT_UNDEFINED 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_SPOT 2
#define LIGHT_POINT 3

//in vec2 st;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D depth_tex;

layout(std140) uniform GlobalMatrices {
  mat4 proj;
  mat4 inv_proj;
  mat4 view;
}
;

struct Light 
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 direction;
  vec4 position;
  int type;
};

layout(std140) uniform Lights
{
  int num_lights;
  Light lights[MAX_LIGHTS];
};

uniform int light_index;

out vec4 frag_color;

vec3 d_Ls = vec3 (1.0, 1.0, 1.0); // white specular colour
vec3 d_Ld = vec3 (0.7, 0.7, 0.7); // dull white diffuse light colour
vec3 d_La = vec3 (0.2, 0.2, 0.2); // grey ambient colour

vec3 kd = vec3 (0.9, 0.9, 0.9);
vec3 ks = vec3 (0.5, 0.5, 0.5);

float specular_exponent = 200.0;

vec3 phong (in vec3 op_eye, in vec3 n_eye) {
  Light light = lights[light_index];

  //vec3 lp_eye = (view * light.position).xyz;
  vec3 lp_eye = (view * vec4(0, 10, 0, 1)).xyz;
  vec3 dist_to_light_eye = lp_eye - op_eye;
  vec3 direction_to_light_eye = normalize (dist_to_light_eye);

  // standard diffuse light
  float dot_prod = max(dot (direction_to_light_eye,  n_eye), 0.0);

  vec3 Id = d_Ld * dot_prod; // final diffuse intensity

  return Id;
}

vec3 func_positon_from_depth(in vec2 st)
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

  vec2 st;
  st.s = gl_FragCoord.x / 1280.0;
  st.t = gl_FragCoord.y / 720.0;

  vec4 p_texel = texture2D(position_tex, st);
  vec4 n_texel = texture2D(normal_tex, st);
  vec4 d_texel = texture2D(depth_tex, st);

 // p_texel.rgb = func_positon_from_depth(st);


  frag_color.rgb =  p_texel.rgb;
  //frag_color.rgb = vec3(0, 1, 0);
  //frag_color.rgb = phong (p_texel.rgb, normalize(n_texel.rgb));
//  frag_color.rgb =  d_texel.rgb;
//    frag_color.rgb =  p_texel.rgb;
//  frag_color.rgb =  p_texel.rgb - func_positon_from_depth(st);
  //frag_color.rgb = normalize(n_texel.rgb);
  frag_color.a = 1.0;
  if (d_texel.x > -0.0001) {
  //  frag_color.rgb = vec3(0, 0, 1);
  //  frag_color.rgb = n_texel.rgb;
   // frag_color.rgb = phong (p_texel.rgb, normalize(n_texel.rgb));
  }
}

