#version 330
#define MAX_LIGHTS 8
#define LIGHT_UNDEFINED 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_SPOT 2
#define LIGHT_POINT 3

in vec2 st;
in vec3 view_dir_tan;
in vec3 light_dir_tan;
in vec3 position_eye;
in vec3 normal_eye;

in mat4 model_view;
in mat3 normal_matrix;
in vec3 position;
in vec3 normal;

// Cubemap (skybox)
in vec3 str;

uniform sampler2D diffuse_texture;
uniform sampler2D normal_texture;
uniform sampler2D specular_texture;
uniform samplerCube cube_texture;

layout(std140) uniform GlobalMatrices {
  mat4 proj;
  mat4 inv_proj;
  mat4 view;
};

layout(std140) uniform Matrices {
  mat4 model;
};

layout(std140) uniform Armature {
  mat4 skinning[64];
};

layout(std140) uniform Material {
  vec4 Ka;
  vec4 Kd;
  vec4 Ks;
  float shininess;
};

layout(std140) uniform Node_State {
  int state_animated;
  int state_debug;
  int state_diffuse;
  int state_diffuse_normal;
  int state_diffuse_specular_normal;
  int state_cubemap_reflect;
  int state_cubemap_skybox;
  int state_standard;
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


layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec3 foo_pos;

vec3 Ls = vec3 (1.0, 1.0, 1.0); // white specular colour
vec3 Ld = vec3 (0.7, 0.7, 0.7); // dull white diffuse light colour
vec3 La = vec3 (0.2, 0.2, 0.2); // grey ambient colour


// -----------------------------------------------------------------
// -----------------------------------------------------------------
// -----------------------------------------------------------------


vec3 func_cubemap_reflect()
{
  vec3 incident_eye = normalize(position_eye);
  vec3 normal = normalize(normal_eye);
  vec3 reflected = reflect(incident_eye, normal);

  // convert from eye to world space
  reflected = vec3(inverse(view + mat4(1.0) * 0.01) * vec4(reflected, 0.0));
  return texture (cube_texture, reflected).rgb;
}


vec3 func_cubemap_skybox()
{
  return texture(cube_texture, str).rgb;
}


vec3 func_light_directional(Light light)
{
  vec3 light_direction_wor = normalize(vec3(light.direction));
//  vec3 light_direction_wor = normalize(vec3(0, -1, 0));
  vec3 light_direction_eye = (view * vec4(light_direction_wor, 0.0)).xyz;
  vec3 direction_to_light_eye = -light_direction_eye;

  return direction_to_light_eye;
}

vec3 func_light_point(Light light)
{
  vec3 light_position_eye = vec3(view * light.position);
  vec3 distance_to_light_eye = light_position_eye - position_eye;
  vec3 direction_to_light_eye = normalize(distance_to_light_eye);

  return direction_to_light_eye;
}


vec3 func_light_apply_all(vec3 material_diffuse)
{
  vec3 d_Ks = vec3 (.1, .1, .1); // fully reflect specular light
  vec3 d_Ka = vec3 (.2, .2, .2); // fully reflect ambient light

  vec3 d_Ls = vec3 (1.0, 1.0, 1.0); // white specular colour
  vec3 d_Ld = vec3 (0.7, 0.7, 0.7); // dull white diffuse light colour
  vec3 d_La = vec3 (0.2, 0.2, 0.2); // grey ambient colour


  float specular_exponent = 60.0;
  float spot_exponent = 50;
  float spot_cutoff = 90.0;
  //vec3 spot_direction = vec3(view * vec4(0, -1, 0, 0));

  vec3 ret = vec3(0, 0, 0);
  for (int i = 0; i < num_lights; i++) {
    float spot_factor = 1.0;
    Light light = lights[i];
    vec3 spot_direction = vec3(view * light.direction);
    vec3 direction_to_light_eye = vec3(0, 0, 0);
  //  vec3 spot_direction = vec3(light.direction);

    if (light.type == LIGHT_DIRECTIONAL) {
      direction_to_light_eye = func_light_directional(light);
    } else if (light.type == LIGHT_POINT) {
      direction_to_light_eye = func_light_point(light);
    } if (light.type == LIGHT_SPOT) {
      direction_to_light_eye = func_light_point(light);
      vec3 s = normalize(vec3(view * light.position) - position_eye);
      float angle = acos(dot(-s, spot_direction));
      float cutoff = radians(clamp(spot_cutoff, 0.0, 90.0));

      if (angle < cutoff) {
        spot_factor = pow(dot(-s, spot_direction), spot_exponent);
      } else {
        spot_factor = 0.0;
      }
    }

    // Ambient intensity
    vec3 Ia = d_La * d_Ka;

    // Diffuse intensity
    float dot_prod = dot(direction_to_light_eye, normalize(normal_eye));
    dot_prod = max(dot_prod, 0.0);
    vec3 Id = d_Ld * material_diffuse * dot_prod; 
    Id *= spot_factor;

    // Specular intensity
    vec3 surface_to_viewer_eye = normalize (-position_eye);

    // blinn
    vec3 half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
    float dot_prod_specular = max(dot (half_way_eye, normalize(normal_eye)), 0.0);
    float specular_factor = pow(dot_prod_specular, specular_exponent);

    vec3 Is = d_Ls * d_Ks * specular_factor; // final specular intensity
    Is *= spot_factor;

    ret += vec3(Ia + Id + Is);
    //ret += vec3(spot_factor, spot_factor, spot_factor);
  }

  return ret;
}


vec3 func_phong_specular_normal()
{
  vec3 diffuse = texture(diffuse_texture, st).rgb;
  vec3 specular = texture(specular_texture, st).rgb;
  vec3 Ia = vec3 (0.2, 0.2, 0.2);

  // sample the normal map and covert from 0:1 range to -1:1 range
  vec3 normal_tan = texture (normal_texture, st).rgb;
  normal_tan = normalize (normal_tan * 2.0 - 1.0);

  // diffuse light equation done in tangent space
  vec3 direction_to_light_tan = normalize (-light_dir_tan);
  float dot_prod = dot (direction_to_light_tan, normal_tan);
  dot_prod = max (dot_prod, 0.0);
  vec3 Id =  Ld * diffuse * dot_prod;

  // specular light equation done in tangent space
  vec3 reflection_tan = reflect (normalize (light_dir_tan), normal_tan);
  float dot_prod_specular = dot (reflection_tan, normalize (view_dir_tan));
  dot_prod_specular = max (dot_prod_specular, 0.0);
  float specular_factor = pow (dot_prod_specular, 100.0);
  vec3 Is = Ls * specular * specular_factor;

  // phong light output
  return Is + Id + Ia;
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
// -----------------------------------------------------------------

vec3 func_diffuse_texture()
{
  //return func_ads(vec3(0, 0, 0), texture(diffuse_texture, st).rgb, vec3(1, 0.5, 0.5), 80);
  //return func_phong(vec3(0,0,0), texture(diffuse_texture, st).rgb, vec3(1, 0.5, 0.5), 50);
  //return func_toon(vec3(0,0,0), texture(diffuse_texture, st).rgb);
  //return func_diffuse(texture(diffuse_texture, st).rgb);
  return vec3(0.7, 0.7, 0.7);
}


vec3 func_standard()
{
 //return func_phong(vec3(0,0,0), vec3(Kd), vec3(1, 0.5, 0.5), 80);
 //return func_ads(vec3(0,0,0), vec3(Kd), vec3(1, 0.5, 0.5), 80);
 //return func_toon(vec3(0,0,0), Kd);
 //return func_diffuse(vec3(Kd));
  return vec3(0.7, 0.7, 0.7);
}


void main()
{
  vec3 out_color = vec3(0.7, 0.7, 0.7);

  if (state_diffuse == 1) 
  {
    out_color = func_light_apply_all(texture(diffuse_texture, st).rgb);
  } 
  else if (state_diffuse_specular_normal == 1) 
  {
    out_color = func_light_apply_all(texture(diffuse_texture, st).rgb);
  } 
  else if (state_cubemap_skybox == 1) 
  {
    out_color = func_cubemap_skybox();
  } 
  else if (state_cubemap_reflect == 1) 
  {
    out_color = func_light_apply_all(func_cubemap_reflect());
  } 
  else if (state_standard == 1) 
  {
    out_color = func_light_apply_all(vec3(Kd));
  }

  frag_color.rgb = out_color;
  frag_color.a = 1.0;

//  foo_pos = normal_eye;
  foo_pos = position_eye;
}
