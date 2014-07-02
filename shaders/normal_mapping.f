#version 330


layout(location = 0) out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec4 lightPosEye;
in vec2 f_textureCoord;
in vec3 LightDir;
in vec3 ViewDir;

uniform sampler2D ColorTex;
uniform sampler2D NormalMapTex;
uniform sampler2D SpecularTex;

vec3 phongModel(vec3 norm, vec3 diffR)
{
  vec4 specular = texture(SpecularTex, f_textureCoord);
  vec3 Ka = vec3(0.0, 0.0, 0.0);
  vec3 Kd = vec3(0.6, 0.6, 0.6);
  vec3 Ks = vec3(0.1, 0.1, 0.1);
  //vec3 Ks = vec3(specular.x, specular.y, specular.z);
  vec3 intensity = vec3(1.0, 1.0, 1.0);
  float shininess = 1;

  vec3 r = reflect(-LightDir, norm);
  vec3 ambient = intensity * Ka;
  float sDotN = max(dot(LightDir, norm), 0.0);
  vec3 diffuse = intensity * diffR * sDotN;
  vec3 spec = vec3(0.0);
  if (sDotN  > 0.0) {
    spec = intensity * Ks * pow(max(dot(r, ViewDir), 0.0), shininess);
  }

  return ambient + diffuse + spec;
}

void main() {
   vec4 normal = texture(NormalMapTex, f_textureCoord);
   vec4 texColor = texture(ColorTex, f_textureCoord);
   FragColor = vec4(phongModel(normal.xyz, texColor.rgb), 1.0);
}
