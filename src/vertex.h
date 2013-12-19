#ifndef VERTEX_H
#define VERTEX_H

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec4 weights;
  glm::vec2 uv;
  glm::ivec4 bones;
};


#endif
