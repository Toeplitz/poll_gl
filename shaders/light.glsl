
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

  return (Id + Is) * atten_factor;
  //return (Id + Is);

  //return Id;
}

