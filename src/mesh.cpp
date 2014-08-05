#include "aabb.h"
#include "mesh.h"
#include "utils.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Mesh::Mesh():
  model(glm::mat4(1.f))
{
  mode = GL_TRIANGLES;
}

Mesh::~Mesh(void)
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/

void Mesh::aabb_generate_bounding() 
{

  glm::vec3 minimum(std::numeric_limits<float>::max());
  glm::vec3 maximum(std::numeric_limits<float>::min());

  for (auto &v : positions) {
    glm::vec3 p = glm::vec3(model * glm::vec4(v, 1.f));
    //  std::cout << glm::to_string(p) << std::endl;
    minimum = glm::min(minimum, p);
    maximum = glm::max(maximum, p);
  }

  // std::cout << "minimum: " << minimum.x << ", " << minimum.y << ", " << minimum.z << std::endl;
  // std::cout << "maximum: " << maximum.x << ", " << maximum.y << ", " << maximum.z << std::endl;

  aabb.setBounds(minimum.x, minimum.y, minimum.z,
      maximum.x, maximum.y, maximum.z);
}


const std::vector<glm::vec3> &Mesh::bone_weights_get() const
{
  return bone_weights;
}


void Mesh::cube_generate(const float &size)
{
  GLfloat cube_vertices[] = {
    // front
    -1.0, -1.0,  1.0,
    1.0, -1.0,  1.0,
    1.0,  1.0,  1.0,
    -1.0,  1.0,  1.0,
    // back
    -1.0, -1.0, -1.0,
    1.0, -1.0, -1.0,
    1.0,  1.0, -1.0,
    -1.0,  1.0, -1.0,
  };

  GLushort cube_elements[] = {
    // front
    0, 1, 2,
    2, 3, 0,
    // top
    3, 2, 6,
    6, 7, 3,
    // back
    7, 6, 5,
    5, 4, 7,
    // bottom
    4, 5, 1,
    1, 0, 4,
    // left
    4, 0, 3,
    3, 7, 4,
    // right
    1, 5, 6,
    6, 2, 1,
  };

  for (int i = 0; i < 8 * 3; i = i + 3) {
    positions_add(glm::vec3(cube_vertices[i], 
          cube_vertices[i + 1], cube_vertices[i + 2]) * size);
  }
  for (int i = 0; i < 12 * 3; i++) {
    indices.push_back(cube_elements[i]);
  }
}


const std::vector<GLshort> &Mesh::indices_get() const
{
  return indices;
}


void Mesh::positions_add(const glm::vec3 &v)
{
  positions.push_back(v);
}


const std::vector<glm::vec3> &Mesh::positions_get() const
{
  return positions;
}


void Mesh::print(const int indent_level)
{
  indent(std::cout, indent_level);

  std::cout << "\tvertices: " << num_vertices_get() << ", indices: " << num_indices_get() << std::endl;
}


unsigned int Mesh::num_indices_get() {
  return indices.size();
}


unsigned int Mesh::num_vertices_get() {
  return positions.size();
}


const std::vector<glm::vec3> &Mesh::bitangents_get() const
{
  return bitangents;
}


const std::vector<glm::ivec3> &Mesh::bone_indices_get() const
{
  return bone_indices;
}




const std::vector<glm::vec3> &Mesh::normals_get() const
{
  return normals;
}


const std::vector<glm::vec3> &Mesh::tangents_get() const
{
  return tangents;
}


void Mesh::quad_generate(const float &size)
{
  const int n_vertices = 6;
  GLfloat quad_pos[] = {
    -1.0, -1.0,
     1.0, -1.0,
     1.0,  1.0,
     1.0,  1.0,
    -1.0,  1.0,
    -1.0, -1.0
  };

  for (int i = 0; i < n_vertices * 2; i = i + 2) {
    positions_add(glm::vec3(quad_pos[i], quad_pos[i + 1], 0.0) * size);
  }

}


void Mesh::texture_st_add(const glm::vec2 &v)
{
  texture_st.push_back(v);
}


const std::vector<glm::vec2> &Mesh::texture_st_get() const
{
  return texture_st;
}



