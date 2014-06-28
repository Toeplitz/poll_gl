#version 330

layout(location = 0) in vec4 vertexCoord;
layout(location = 1) in vec4 vertexNormal;
layout(location = 2) in vec4 weights;
layout(location = 3) in ivec4 boneIndex;
layout(location = 4) in vec2 textureCoord;

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

  vec4 lightPos = vec4(50, 100, 0, 1);
  lightPosEye = view * lightPos;

  Position = vec3(modelView * vertexCoord);
  Normal = normalize(normalMatrix * vec3(vertexNormal));

  gl_Position = projection * vec4(Position, 1.0);
  f_textureCoord = textureCoord;
}
