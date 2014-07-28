#version 330

//in vec2 st;


layout(std140) uniform GlobalMatrices 
{
  mat4 proj;
  mat4 inv_proj;
  mat4 view;
};


layout(std140) uniform Light 
{
  vec4 light_position;
};


uniform sampler2D normal_tex;
uniform sampler2D depth_tex;

out vec4 frag_color;

vec3 d_Ls = vec3 (1.0, 1.0, 1.0); // white specular colour
vec3 d_Ld = vec3 (0.7, 0.7, 0.7); // dull white diffuse light colour
vec3 d_La = vec3 (0.2, 0.2, 0.2); // grey ambient colour

vec3 kd = vec3 (0.9, 0.9, 0.9);
vec3 ks = vec3 (0.5, 0.5, 0.5);

float specular_exponent = 100.0;

vec3 phong (in vec3 op_eye, in vec3 n_eye) {
  vec3 lp_eye = (view * vec4(vec3(light_position), 1.0)).xyz;
  //vec3 lp_eye = (view * vec4(0, 20, 0, 1)).xyz;
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


/* Basic FXAA implementation based on the code on geeks3d.com with the
   modification that the texture2DLod stuff was removed since it is
   unsupported by WebGL. */

#define FXAA_REDUCE_MIN   (1.0/ 128.0)
#define FXAA_REDUCE_MUL   (1.0 / 8.0)
#define FXAA_SPAN_MAX     8.0

vec4 applyFXAA(vec2 fragCoord, sampler2D tex)
{
    vec4 color;
    //vec2 inverseVP = vec2(1.0 / uViewportSize.x, 1.0 / uViewportSize.y);
    vec2 inverseVP = vec2(1.0 / 1280, 1.0 / 720);
    vec3 rgbNW = texture2D(tex, (fragCoord + vec2(-1.0, -1.0)) * inverseVP).xyz;
    vec3 rgbNE = texture2D(tex, (fragCoord + vec2(1.0, -1.0)) * inverseVP).xyz;
    vec3 rgbSW = texture2D(tex, (fragCoord + vec2(-1.0, 1.0)) * inverseVP).xyz;
    vec3 rgbSE = texture2D(tex, (fragCoord + vec2(1.0, 1.0)) * inverseVP).xyz;
    vec3 rgbM  = texture2D(tex, fragCoord  * inverseVP).xyz;
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    
    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) *
                          (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
              max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
              dir * rcpDirMin)) * inverseVP;
      
    vec3 rgbA = 0.5 * (
        texture2D(tex, fragCoord * inverseVP + dir * (1.0 / 3.0 - 0.5)).xyz +
        texture2D(tex, fragCoord * inverseVP + dir * (2.0 / 3.0 - 0.5)).xyz);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture2D(tex, fragCoord * inverseVP + dir * -0.5).xyz +
        texture2D(tex, fragCoord * inverseVP + dir * 0.5).xyz);

    float lumaB = dot(rgbB, luma);
    if ((lumaB < lumaMin) || (lumaB > lumaMax))
        color = vec4(rgbA, 1.0);
    else
        color = vec4(rgbB, 1.0);
    return color;
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

  //frag_color.rgb =  vec3(n_texel);
  //frag_color.rgb =  vec3(d_texel, d_texel, d_texel);
  frag_color.rgb = phong(pos_eye, normalize(n_texel.rgb));
 // frag_color.rgb = light_position.rgb;
  //frag_color.rgb = vec3(0, state_cubemap_skybox, 0);
  frag_color.a = 1.0;
  if (d_texel > -0.0001) {
  //  frag_color.rgb = vec3(0, 0, 1);
  //  frag_color.rgb = n_texel.rgb;
   // frag_color.rgb = phong (p_texel.rgb, normalize(n_texel.rgb));
  }
}

