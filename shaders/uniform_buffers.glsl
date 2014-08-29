layout(std140) uniform GlobalMatrices {
  mat4 proj;
  mat4 inv_proj;
  mat4 view;
  mat4 view_projection;
};

layout(std140) uniform Matrices {
  mat4 model;
};

layout(std140) uniform Armature {
  mat4 skinning[64];
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

