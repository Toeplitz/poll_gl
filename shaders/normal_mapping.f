#version 330

uniform sampler2D ColorTex;
uniform sampler2D NormalMapTex;

layout(location = 0) out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec4 lightPosEye;
in vec2 f_textureCoord;

in vec3 LightDir;
in vec3 ViewDir;

vec3 ads() 
{
    vec3 Ka = vec3(0.0, 0.0, 0.0);
    vec3 Kd = vec3(1.0, 1.0, 1.0);
    vec3 Ks = vec3(1.0, 0.5, 0.5);
    vec3 intensity = vec3(1.0, 1.0, 1.0);
    float shininess = 140.0;

    vec3 n = normalize(Normal);
    vec3 s = normalize(vec3(lightPosEye) - Position);
    vec3 v = normalize(vec3(-Position));
    vec3 r = reflect(-s, n);

    return intensity * (Ka + Kd * max(dot(s, n), 0.0) + Ks * pow(max(dot(r,v), 0.0), shininess));
}


vec3 phongModel(vec3 norm, vec3 diffR)
{
    vec3 Ka = vec3(0.0, 0.0, 0.0);
    vec3 Kd = vec3(1.0, 1.0, 1.0);
    vec3 Ks = vec3(1.0, 0.5, 0.5);
    vec3 intensity = vec3(1.0, 1.0, 1.0);
    float shininess = 140.0;

    vec3 r = reflect(-LightDir, norm);
    vec3 ambient = intensity * Ka;
    float sDotN = max(dot(LightDir, norm), 0.0);
    vec3 diffuse = intensity * diffR * sDotN;
    vec3 spec = vec3(0.0);
    if (sDotN > 0.0) {
      spec = intensity * Ks * pow(max(dot(r, ViewDir), 0.0), shininess);
    }

    return ambient + diffuse * spec;
}

void main() {

// ORIGINAL
// vec3 texColor = texture(ColorTex, f_textureCoord).rgb;
// FragColor = vec4(ads() * texColor, 1);

   vec4 normal = texture(NormalMapTex, f_textureCoord);
   vec4 texColor = texture(ColorTex, f_textureCoord);
   FragColor = vec4(phongModel(normal.xyz, texColor.rgb), 1.0);

  //FragColor = vec4(ads(), 1);
  //FragColor = vec4(ads() * vec3(0.7, 0.7, 0.7), 1);
  //FragColor = vec4(f_textureCoord.r, f_textureCoord.g, 1.0, 1);
}
