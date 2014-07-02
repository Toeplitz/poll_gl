#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tanget;
  glm::vec3 tangent;
  glm::vec3 bitangent;
  glm::vec4 weights;
  glm::vec2 uv;
  glm::ivec4 bones;
};

