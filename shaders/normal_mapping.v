#version 330

layout(location = 0) in vec4 vertexCoord;
layout(location = 1) in vec4 vertexNormal;
layout(location = 2) in vec4 tangent;
layout(location = 3) in vec4 bitangent;
layout(location = 4) in vec4 weights;
layout(location = 5) in ivec4 boneIndex;
layout(location = 6) in vec2 textureCoord;

layout(std140) uniform GlobalMatrices {
  mat4 projection;
  mat4 view;
};

layout(std140) uniform Matrices {
  mat4 model;
};

layout(std140) uniform Armature {
  mat4 skinning[64];
};


layout(std140) uniform Debug {
  vec4 debug;
};


out vec3 Position;
out vec3 Normal;
out vec4 lightPosEye;
out vec2 f_textureCoord;
out vec3 LightDir;
out vec3 ViewDir;


void main(void) {
  mat4 animation = 
    weights[0] * skinning[int(boneIndex[0])] +
    weights[1] * skinning[int(boneIndex[1])] +
    weights[2] * skinning[int(boneIndex[2])];

  if (weights[0] == 0.0 && weights[1] == 0.0 && weights[2] == 0.0) {
    animation = model;
  } 

  if (debug.w == 1.0) {
    animation = model;
  }

  mat4 modelView = view * animation;
  mat3 normalMatrix = mat3(transpose(inverse(modelView)));

  vec4 lightPos = vec4(4.2, 5.7, 3.8, 1);
  lightPosEye = view * lightPos;

  Position = vec3(modelView * vertexCoord);
  Normal = normalize(normalMatrix * vec3(vertexNormal));

  // Transform normal and tangent to eye space
  vec3 norm = normalize(normalMatrix * vec3(vertexNormal));
  vec3 tang = normalize(normalMatrix * vec3(tangent));

  // Compute the binormal
  //vec3 binormal = normalize(cross(norm, tang)) * -1;
  vec3 binormal = vec3(bitangent) * -1;

  // Matrix for transformation to tangent space
  mat3 toObjectLocal = mat3(
    tang.x, binormal.x, norm.x,
    tang.y, binormal.y, norm.y,
    tang.z, binormal.z, norm.z);

  // Get the position in eye coordinate
  vec3 pos = vec3(modelView * vertexCoord);

  // Transform light dir. and view dir. to tangent space
  LightDir = normalize(toObjectLocal * (lightPosEye.xyz - pos));
  ViewDir = toObjectLocal * normalize(-pos);

  gl_Position = projection * vec4(pos, 1.0);
  f_textureCoord = textureCoord;
}
