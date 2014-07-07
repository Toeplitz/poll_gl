#include "aabb.h"
#include "mesh.h"
#include "utils.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Mesh::Mesh():
  model(glm::mat4(1.0))
{
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
    glm::vec3 p = glm::vec3(model * glm::vec4(v, 1.0));
 //   std::cout << glm::to_string(p) << std::endl;
    minimum = glm::min(minimum, p);
    maximum = glm::max(maximum, p);
  }

  std::cout << "minimum: " << minimum.x << ", " << minimum.y << ", " << minimum.z << std::endl;
  std::cout << "maximum: " << maximum.x << ", " << maximum.y << ", " << maximum.z << std::endl;

  aabb.setBounds(minimum.x, minimum.y, minimum.z,
                 maximum.x, maximum.y, maximum.z);
}


void Mesh::buffer_data_get(std::vector<glm::vec4> *normals_ptr,
    std::vector<glm::vec4> *tangent_ptr,
    std::vector<glm::vec4> *bitangent_ptr,
    std::vector<glm::vec4> *weights_ptr,
    std::vector<glm::ivec4> *bone_indices_ptr)
{
  std::vector<glm::vec4> tempNormal;
  std::vector<glm::vec4> temp_tangent;
  std::vector<glm::vec4> temp_bitangent;
  std::vector<glm::vec4> tempWeight;
  std::vector<glm::ivec4> tempBoneIndex;
  size_t n = vertices.size();

  for (size_t i = 0; i < n; i++) {
    tempNormal.push_back(glm::vec4(vertices[i].normal, 1.0));
    temp_tangent.push_back(glm::vec4(vertices[i].tangent, 1.0));
    temp_bitangent.push_back(glm::vec4(vertices[i].bitangent, 1.0));
    tempWeight.push_back(vertices[i].weights);
    tempBoneIndex.push_back(vertices[i].bones);
  }

  *normals_ptr = tempNormal;
  *tangent_ptr = temp_tangent;
  *bitangent_ptr = temp_bitangent;
  *weights_ptr = tempWeight;
  *bone_indices_ptr = tempBoneIndex;
}


const std::vector<GLshort> &Mesh::indices_get() const
{
  return indices;
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
  return vertices.size();
}


const std::vector<glm::vec3> &Mesh::positions_get() const
{
  return positions;
}


const std::vector<glm::vec2> &Mesh::texture_st_get() const
{
  return texture_st;
}

