#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "aabb.h"
#include "gldefaults.h"
#include "vertex.h"
#include <vector>

class Mesh {
  private:

  public:
    Aabb aabb;
    std::vector<Vertex> vertices;
    std::vector<GLshort> indices;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texture_st;
    glm::mat4 model;
    glm::mat4 scale_matrix;
    unsigned int num_faces;

    Mesh();
    ~Mesh();

    void                           aabb_generate_bounding();
    void                           buffer_data_get(std::vector<glm::vec4> *normals_ptr,
                                           std::vector<glm::vec4> *tangent_ptr, std::vector<glm::vec4> *bitangent_ptr,
                                           std::vector<glm::vec4> *weights_ptr, std::vector<glm::ivec4> *bone_indices_ptr);
    const std::vector<GLshort>    &indices_get() const;
    void                           print(const int indent_level);
    unsigned int                   num_indices_get();
    unsigned int                   num_vertices_get();
    const std::vector<glm::vec3>  &positions_get() const;
    const std::vector<glm::vec2>  &texture_st_get() const;
};

