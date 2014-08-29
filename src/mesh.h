#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gldefaults.h"
#include <vector>

class Mesh {
  private:

  public:
    GLenum mode = GL_TRIANGLES;
    GLuint gl_vao = 0;
    GLuint gl_vertex_buffers[8];

    std::vector<GLshort> indices;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<glm::vec2> texture_st;
    std::vector<glm::vec3> bone_weights;
    std::vector<glm::ivec3> bone_indices;

    glm::mat4 scale_matrix;
    unsigned int num_faces;

    const std::vector<glm::vec3>   &bitangents_get() const;
    const std::vector<glm::ivec3>  &bone_indices_get() const;
    const std::vector<glm::vec3>   &bone_weights_get() const;
    void                            generate_cube(const float &size);
    void                            generate_pyramid(const float &size);
    void                            generate_quad(const float &size);
    const std::vector<GLshort>     &indices_get() const;
    void                            positions_add(const glm::vec3 &v);
    void                            positions_clear();
    const std::vector<glm::vec3>   &positions_get() const;
    void                            positions_update(const glm::mat4 &model);
    void                            print(const int indent_level);
    const std::vector<glm::vec3>   &normals_get() const;
    unsigned int                    num_indices_get();
    unsigned int                    num_texture_st_get();
    unsigned int                    num_vertices_get();
    const std::vector<glm::vec3>   &tangents_get() const;
    const std::vector<glm::vec2>   &texture_st_get() const;
    void                            texture_st_clear();
    void                            texture_st_add(const glm::vec2 &v);
};

