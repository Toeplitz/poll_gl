#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "aabb.h"


class Mesh {
  private:
    Aabb generateBoundingAabb() const;

  public:
    Aabb aabb;
    std::vector<Vertex> vertices;
    std::vector<GLshort> indices;
    glm::mat4 model;


    Mesh();
    ~Mesh();

    void buffer_data_get(std::vector<glm::vec4> *vertices_ptr, 
                         std::vector<glm::vec4> *normals_ptr,
                         std::vector<glm::vec4> *weights_ptr,
                         std::vector<glm::ivec4> *bone_indices_ptr,
                         std::vector<glm::vec2> *uv_ptr,
                         std::vector<GLshort> *indices_ptr);

    const unsigned int getSize();
};

#endif
