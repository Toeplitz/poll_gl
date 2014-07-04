#version 330

in vec2 st;
in vec3 view_dir_tan;
in vec3 light_dir_tan;
in vec3 position_eye;
in vec3 normal_eye;


uniform sampler2D diffuse_texture;
uniform sampler2D normal_texture;
uniform sampler2D specular_texture;


layout(std140) uniform GlobalMatrices {
  mat4 proj;
  mat4 view;
};

layout(std140) uniform Material {
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
  float shininess;
};


layout(std140) uniform State {
  int state_animated;
  int state_debug;
  int state_diffuse;
  int state_diffuse_normal;
  int state_diffuse_specular_normal;
  int state_standard;
};


out vec4 frag_color;

vec3 light_position_world  = vec3 (-10.0, 10.0, -4.0);
vec3 Ls = vec3 (1.0, 1.0, 1.0); // white specular colour
vec3 Ld = vec3 (0.7, 0.7, 0.7); // dull white diffuse light colour
vec3 La = vec3 (0.2, 0.2, 0.2); // grey ambient colour


vec3 func_ads(vec3 ambient, vec3 diffuse, vec3 specular, float shine) {
    vec3 intensity = vec3(1.0, 1.0, 1.0);

    vec3 light_position_eye = vec3(view * vec4 (light_position_world, 1.0));

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
  vec3 light_position_eye = vec3(view * vec4 (light_position_world, 1.0));
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

vec3 func_diffuse_texture()
{
  return func_ads(vec3(0,0,0), texture(diffuse_texture, st).rgb, vec3(1,1,1), 20);
}


vec3 func_standard()
{
  return func_phong(vec3(0,0,0), Kd, vec3(1, 0.5, 0.5), 80);
 // return func_ads(vec3(0,0,0), Kd, vec3(1, 0.5, 0.5), 80);
}


void main()
{
  vec3 out_color = vec3(0.5, 0.5, 0.5);

  if (state_diffuse == 1) {
    out_color = func_diffuse_texture();
  } else if (state_standard == 1) {
    out_color = func_standard();
  }

  frag_color.rgb = out_color;
  frag_color.a = 1.0;
}
