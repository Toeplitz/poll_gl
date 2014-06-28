#version 330

uniform sampler2D tex;

layout(location = 0) out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec4 lightPosEye;
in vec2 f_textureCoord;


vec3 ads() {
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


void main() {
  vec3 texColor = texture(tex, f_textureCoord).rgb;
  FragColor = vec4(ads() * texColor, 1);
  //FragColor = vec4(ads(), 1);
  //FragColor = vec4(vec3(0.5, 0.5, 0.5), 1);
  //FragColor = vec4(f_textureCoord.r, f_textureCoord.g, 1.0, 1);
}
