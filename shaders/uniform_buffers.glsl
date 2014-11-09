layout(std140) uniform GlobalMatrices {
  mat4 proj;
  mat4 inv_proj;
  mat4 view;
  mat4 view_projection;
};

layout(std140) uniform Matrices {
  mat4 model;
  vec4 animated;
};

layout(std140) uniform Armature {
  mat4 skinning[64];
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
  mat4 light_projection;
  mat4 light_view;
  int light_type;
};

layout(std140) uniform Material
{
  vec4 material_ka;
  vec4 material_kd;
  vec4 material_ks;
  float material_shininess;
};
