layout(location = 0) in vec4 vertexCoord;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec4 weights;
layout(location = 3) in ivec4 boneIndex;

uniform mat4 vp;
uniform mat4 model;

uniform mat4 skinning[64];


void main(void) {
  mat4 animation = 
    weights[0] * skinning[int(boneIndex[0])] +
    weights[1] * skinning[int(boneIndex[1])] +
    weights[2] * skinning[int(boneIndex[2])];

  if (weights[0] == 0 && weights[1] == 0 && weights[2] == 0) {
    gl_Position = vp * model * vertexCoord;
  } else {
    gl_Position = vp * animation * vertexCoord;
  }
}
