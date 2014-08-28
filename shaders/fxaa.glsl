#define FXAA_REDUCE_MIN   (1.0/ 128.0)
#define FXAA_REDUCE_MUL   (1.0 / 8.0)
#define FXAA_SPAN_MAX     8.0

// FXAA shader, GLSL code adapted from:
// http://horde3d.org/wiki/index.php5?title=Shading_Technique_-_FXAA
// Whitepaper describing the technique:
// http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf

vec4 fxaa(vec2 fragCoord, sampler2D tex)
{
    vec4 color;
    vec2 texCoordOffset = vec2(1.0 / 1280.0, 1.0 / 720.0);

    vec3 luma = vec3(0.299, 0.587, 0.114); 
    float lumaTL = dot(luma, texture2D(tex, fragCoord.xy + (vec2(-1.0, -1.0) * texCoordOffset)).xyz);
    float lumaTR = dot(luma, texture2D(tex, fragCoord.xy + (vec2(1.0, -1.0) * texCoordOffset)).xyz);
    float lumaBL = dot(luma, texture2D(tex, fragCoord.xy + (vec2(-1.0, 1.0) * texCoordOffset)).xyz);
    float lumaBR = dot(luma, texture2D(tex, fragCoord.xy + (vec2(1.0, 1.0) * texCoordOffset)).xyz);
    float lumaM  = dot(luma, texture2D(tex, fragCoord.xy).xyz); 

    return vec4(lumaTL + lumaTR + lumaBL + lumaBR + lumaM);
}
