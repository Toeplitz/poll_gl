#version 330
#define MAX_LIGHTS 8
#define LIGHT_UNDEFINED 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_SPOT 2
#define LIGHT_POINT 3

//in vec2 st;

uniform sampler2D normal_tex;
uniform sampler2D depth_tex;

layout(std140) uniform GlobalMatrices {
  mat4 proj;
  mat4 inv_proj;
  mat4 view;
};

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

float specular_exponent = 100.0;

vec3 phong (in vec3 op_eye, in vec3 n_eye) {
  Light light = lights[light_index];

  vec3 lp_eye = (view * vec4(vec3(light.position), 1)).xyz;
 // vec3 lp_eye = (view * vec4(0, 20, 0, 1)).xyz;
  vec3 dist_to_light_eye = lp_eye - op_eye;
  vec3 direction_to_light_eye = normalize (dist_to_light_eye);

  // standard diffuse light
  float dot_prod = max(dot (direction_to_light_eye,  n_eye), 0.0);

  vec3 Id = d_Ld * dot_prod; // final diffuse intensity
  // standard specular light
  vec3 reflection_eye = reflect (-direction_to_light_eye, n_eye);
  vec3 surface_to_viewer_eye = normalize (-op_eye);
  float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
  dot_prod_specular = max (dot_prod_specular, 0.0);
  float specular_factor = pow (dot_prod_specular, specular_exponent);
  vec3 Is = d_Ls * ks * specular_factor; // final specular intensity

  float dist_2d = max (0.0, 1.0 - distance (lp_eye, op_eye) / 50.0);
  float atten_factor =  dist_2d;

  //return vec3(dist_2d,dist_2d,dist_2d);

  return (Id + Is) * atten_factor;

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

void main () 
{

  vec2 st;
  st.s = gl_FragCoord.x / 1280.0;
  st.t = gl_FragCoord.y / 720.0;

  vec4 n_texel = texture2D(normal_tex, st);
  float d_texel = texture2D(depth_tex, st).r;

  vec3 p_texel = reconstruct_position(d_texel, st);
  vec3 pos_eye = vec3(view * vec4(p_texel.rgb, 1.0));

  frag_color.rgb =  p_texel.rgb;
  frag_color.rgb = phong (pos_eye, normalize(n_texel.rgb));
  frag_color.a = 1.0;
  if (d_texel > -0.0001) {
  //  frag_color.rgb = vec3(0, 0, 1);
  //  frag_color.rgb = n_texel.rgb;
   // frag_color.rgb = phong (p_texel.rgb, normalize(n_texel.rgb));
  }
}

