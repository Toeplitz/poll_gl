
const vec2 poisson16[] = vec2[] (
  // Poisson Disk Samples
  vec2( -0.94201624,  -0.39906216 ),
  vec2(  0.94558609,  -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2(  0.34495938,   0.29387760 ),
  vec2( -0.91588581,   0.45771432 ),
  vec2( -0.81544232,  -0.87912464 ),
  vec2( -0.38277543,   0.27676845 ),
  vec2(  0.97484398,   0.75648379 ),
  vec2(  0.44323325,  -0.97511554 ),
  vec2(  0.53742981,  -0.47373420 ),
  vec2( -0.26496911,  -0.41893023 ),
  vec2(  0.79197514,   0.19090188 ),
  vec2( -0.24188840,   0.99706507 ),
  vec2( -0.81409955,   0.91437590 ),
  vec2(  0.19984126,   0.78641367 ),
  vec2(  0.14383161,  -0.14100790 )
);


float ssoa(vec2 st, float d_texel, vec3 pos_eye, vec3 n_texel)
{
  vec2 filterRadius = vec2(config_ssoa.filter_radius / config_viewport.width,
    config_ssoa.filter_radius / config_viewport.height);
  float ambientOcclusion = 0;

  for (int i = 0; i < config_ssoa.sample_count; ++i)
  {
      // sample at an offset specified by the current Poisson-Disk sample and scale it by a radius (has to be in Texture-Space)
      vec2 sampleTexCoord = st + (poisson16[i] * (filterRadius));
      float sampleDepth = texture(depth_tex, sampleTexCoord).r;

      vec3 samplePos = vec3(view * vec4(reconstruct_position(sampleDepth, sampleTexCoord), 1.0));
      vec3 sampleDir = normalize(samplePos - pos_eye);

      // angle between SURFACE-NORMAL and SAMPLE-DIRECTION (vector from SURFACE-POSITION to SAMPLE-POSITION)
      float NdotS = max(dot(n_texel, sampleDir), 0);
      // distance between SURFACE-POSITION and SAMPLE-POSITION
      float VPdistSP = distance(pos_eye, samplePos);

      // a = distance function
      float a = 1.0 - smoothstep(config_ssoa.distance_threshold, config_ssoa.distance_threshold * 2, VPdistSP);
      // b = dot-Product
      float b = NdotS;

      ambientOcclusion += (a * b);
  }

  return 1 - ambientOcclusion / config_ssoa.sample_count;
}
