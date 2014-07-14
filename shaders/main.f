#version 330

in vec2 st;
in vec3 view_dir_tan;
in vec3 light_dir_tan;
in vec3 light_position_eye;
in vec3 position_eye;
in vec3 normal_eye;

// Cubemap (skybox)
in vec3 str;

uniform sampler2D diffuse_texture;
uniform sampler2D normal_texture;
uniform sampler2D specular_texture;
uniform samplerCube cube_texture;

layout(std140) uniform GlobalMatrices {
  mat4 proj;
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


out vec4 frag_color;

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
  reflected = vec3(inverse(view) * vec4(reflected, 0.0));
  return texture (cube_texture, reflected).rgb;
}


vec3 func_cubemap_skybox()
{
  return texture(cube_texture, str).rgb;
}


vec3 func_diffuse(vec3 diffuse)
{
  vec3 Ia = vec3 (1, 1, 1);

  vec3 s = normalize(vec3(light_position_eye) - position_eye);
  float sDotN = max(dot(s, normal_eye), 0.0);
  return Ia * diffuse * sDotN;
}


vec3 func_ads(vec3 ambient, vec3 diffuse, vec3 specular, float shine) {
    vec3 intensity = vec3(1.0, 1.0, 1.0);

    vec3 n = normalize(normal_eye);
    vec3 s = normalize(vec3(light_position_eye) - position_eye);
    vec3 v = normalize(vec3(-position_eye));
    vec3 r = reflect(-s, n);

    return intensity * (ambient + diffuse * max(dot(s, n), 0.0) + specular * pow(max(dot(r,v), 0.0), shine));
}


vec3 func_phong(vec3 ambient, vec3 diffuse, vec3 specular, float shine)
{
  // ambient intensity
  vec3 Ia = La * ambient;

  // diffuse intensity
  // raise light position to eye space
  vec3 distance_to_light_eye = light_position_eye - position_eye;
  vec3 direction_to_light_eye = normalize(distance_to_light_eye);
  float dot_prod = dot(direction_to_light_eye, normal_eye);
  dot_prod = max(dot_prod, 0.0);
  vec3 Id = Ld * diffuse * dot_prod; // final diffuse intensity

  // specular intensity
  vec3 surface_to_viewer_eye = normalize (-position_eye);

  //vec3 reflection_eye = reflect (-direction_to_light_eye, normal_eye);
  //float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
  //dot_prod_specular = max (dot_prod_specular, 0.0);
  //float specular_factor = pow (dot_prod_specular, shine);

  // blinn
  vec3 half_way_eye = normalize (surface_to_viewer_eye + direction_to_light_eye);
  float dot_prod_specular = max (dot (half_way_eye, normal_eye), 0.0);
  float specular_factor = pow (dot_prod_specular, shine);

  vec3 Is = Ls * specular * specular_factor; // final specular intensity

  // final colour
  return Is + Id + Ia;
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


vec3 func_toon(vec3 ambient, vec3 diffuse)
{
  vec3 Ia = vec3 (0.5, 0.5, 0.5);
  int levels = 3;
  float scale_factor = 1;

  vec3 s = normalize(light_position_eye - position_eye);
  float cosine = max(0.0, dot(s, normal_eye));
  vec3 d = diffuse * floor(cosine * levels) * scale_factor;

  return Ia * (ambient + d);
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
// -----------------------------------------------------------------

vec3 func_diffuse_texture()
{
  //return func_ads(vec3(0, 0, 0), texture(diffuse_texture, st).rgb, vec3(1, 1, 1), 20);
  return func_diffuse(texture(diffuse_texture, st).rgb);
}


vec3 func_standard()
{
 //return func_phong(vec3(0,0,0), vec3(Kd), vec3(1, 0.5, 0.5), 80);
 //return func_ads(vec3(0,0,0), Kd, vec3(1, 0.5, 0.5), 80);
 //return func_toon(vec3(0,0,0), Kd);
 return func_diffuse(vec3(Kd));
 }


void main()
{
  vec3 out_color = vec3(0.7, 0.7, 0.7);

  if (state_diffuse == 1) 
  {
    out_color = func_diffuse_texture();
  } 
  else if (state_diffuse_specular_normal == 1) 
  {
    out_color = func_phong_specular_normal();
  } 
  else if (state_cubemap_skybox == 1) 
  {
    out_color = func_cubemap_skybox();
  } 
  else if (state_cubemap_reflect == 1) 
  {
  //out_color = func_diffuse(func_cubemap_reflect());
    out_color = func_cubemap_reflect();
  } 
  else if (state_standard == 1) 
  {
    out_color = func_standard();
  }

  frag_color.rgb = out_color;
  frag_color.a = 1.0;
}
