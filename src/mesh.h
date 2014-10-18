#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gldefaults.h"
#include <vector>


using glm::vec2;
using glm::ivec3;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class Mesh {
  private:

  public:
    GLenum mode = GL_TRIANGLES;
    GLuint gl_vao = 0;
    GLuint gl_vertex_buffers[8];

    std::vector<GLshort> indices;
    std::vector<vec3> positions;
    std::vector<vec3> normals;
    std::vector<vec3> tangents;
    std::vector<vec3> bitangents;
    std::vector<vec2> texture_st;
    std::vector<vec3> bone_weights;
    std::vector<ivec3> bone_indices;

    unsigned int num_faces;

    const std::vector<vec3>    &bitangents_get() const;
    const std::vector<ivec3>   &bone_indices_get() const;
    const std::vector<vec3>    &bone_weights_get() const;
    void                        generate_line_cube(const float &size);
    void                        generate_cube(const float &size);
    void                        generate_pyramid(const float &size);
    void                        generate_quad(const float &size);
    const std::vector<GLshort> &indices_get() const;
    void                        positions_add(const vec3 &v);
    void                        positions_clear();
    const std::vector<vec3>    &positions_get() const;
    void                        positions_update(const mat4 &model);
    void                        print(const int indent_level);
    const std::vector<vec3>    &normals_get() const;
    unsigned int                num_indices_get();
    unsigned int                num_texture_st_get();
    unsigned int                num_vertices_get();
    const std::vector<vec3>    &tangents_get() const;
    const std::vector<vec2>    &texture_st_get() const;
    void                        texture_st_clear();
    void                        texture_st_add(const vec2 &v);
};

