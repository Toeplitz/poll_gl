
#define LIGHT_UNDEFINED 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_SPOT 2
#define LIGHT_POINT 3


vec3 light_directional()
{
  vec3 light_direction_wor = normalize(vec3(light_direction));
  vec3 light_direction_eye = (view * vec4(light_direction_wor, 0.0)).xyz;
  vec3 direction_to_light_eye = -light_direction_eye;

  return direction_to_light_eye;
}

vec3 light_point(in vec3 position_eye)
{
  vec3 light_position_eye = vec3(view * light_position);
  vec3 distance_to_light_eye = light_position_eye - position_eye;
  vec3 direction_to_light_eye = normalize(distance_to_light_eye);

  return direction_to_light_eye;
}

vec3 light_apply(in vec3 position_eye, in vec3 normal_eye, vec3 material_diffuse, float shadow)
{
  vec3 d_Ks = vec3 (0.5, 0.5, 0.5); // fully reflect specular light
  vec3 d_Ka = vec3 (0, 0, 0); // fully reflect ambient light
  vec3 d_Kd = vec3(material_diffuse);

  vec3 d_Ls = vec3(0.5, 0.5, 0.5); 
  vec3 d_Ld = vec3(light_color);
  vec3 d_La = vec3(light_color); 

  float specular_exponent = 500.0;
  float spot_exponent = 50;
  float spot_cutoff = 90.0;

  float spot_factor = 1.0;
  vec3 spot_direction = vec3(view * light_direction);
  vec3 direction_to_light_eye = vec3(0, 0, 0);

  if (light_type == LIGHT_DIRECTIONAL) 
  {
    direction_to_light_eye = light_directional();
  } 
    else if (light_type == LIGHT_POINT) 
  {
    direction_to_light_eye = light_point(position_eye);
  } 
    else if (light_type == LIGHT_SPOT) 
  {
    direction_to_light_eye = light_point(position_eye);
    vec3 s = normalize(vec3(view * light_position) - position_eye);
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
  vec3 Id = d_Ld * d_Kd * dot_prod; 
  Id *= spot_factor;

  // Specular intensity
  vec3 surface_to_viewer_eye = normalize (-position_eye);

  // blinn
  vec3 half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
  float dot_prod_specular = max(dot (half_way_eye, normalize(normal_eye)), 0.0);
  float specular_factor = pow(dot_prod_specular, specular_exponent);

  vec3 Is = d_Ls * d_Ks * specular_factor; // final specular intensity
  Is *= spot_factor;

  return vec3(Ia + shadow * Id + Is);
  //ret += vec3(spot_factor, spot_factor, spot_factor);
}

