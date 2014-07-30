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
    GLenum mode;
    GLuint gl_vao;

    Aabb aabb;
    std::vector<GLshort> indices;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<glm::vec2> texture_st;
    std::vector<glm::vec3> bone_weights;
    std::vector<glm::ivec3> bone_indices;

    glm::mat4 model;
    glm::mat4 scale_matrix;
    unsigned int num_faces;

    Mesh();
    ~Mesh();

    void                            aabb_generate_bounding();
    const std::vector<glm::vec3>   &bitangents_get() const;
    const std::vector<glm::ivec3>  &bone_indices_get() const;
    const std::vector<glm::vec3>   &bone_weights_get() const;
    void                            cube_generate(const float &size);
    void                            cube_generate2(const float &size);
    void                            cube_generate3(const float &size);
    const std::vector<GLshort>     &indices_get() const;
    const std::vector<glm::vec3>   &positions_get() const;
    void                            print(const int indent_level);
    const std::vector<glm::vec3>   &normals_get() const;
    unsigned int                    num_indices_get();
    unsigned int                    num_vertices_get();
    const std::vector<glm::vec3>   &tangents_get() const;
    const std::vector<glm::vec2>   &texture_st_get() const;
    void                            sphere_generate(const float radius, const unsigned int rings, const unsigned int sectors);
    void                            quad_generate(const float &size);
};

