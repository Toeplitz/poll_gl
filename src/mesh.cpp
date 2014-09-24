#include "mesh.h"
#include "utils.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


const std::vector<vec3> &Mesh::bone_weights_get() const
{
  return bone_weights;
}


void Mesh::generate_cube(const float &size)
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
    positions_add(vec3(cube_vertices[i], 
          cube_vertices[i + 1], cube_vertices[i + 2]) * size);
  }
  for (int i = 0; i < 12 * 3; i++) {
    indices.push_back(cube_elements[i]);
  }
}


void Mesh::generate_pyramid(const float &size)
{
  positions_add(size * vec3(0.0f, 1.f, 0.0f));
  positions_add(size * vec3(-1.0f, -1.0f, 1.0f));
  positions_add(size * vec3(1.0f, -1.0f, 1.0f));
  positions_add(size * vec3(0.0f, 1.0f, 0.0f));
  positions_add(size * vec3(-1.0f, -1.0f, 1.0f));
  positions_add(size * vec3(0.0f, -1.0f, -1.0f));
  positions_add(size * vec3(0.0f, 1.0f, 0.0f));
  positions_add(size * vec3(0.0f, -1.0f, -1.0f));
  positions_add(size * vec3(1.0f, -1.0f, 1.0f));
  positions_add(size * vec3(-1.0f, -1.0f, 1.0f));
  positions_add(size * vec3(0.0f, -1.0f, -1.0f));
  positions_add(size * vec3(1.0f, -1.0f, 1.0f));
}

void Mesh::generate_quad(const float &size)
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
    positions_add(vec3(quad_pos[i], quad_pos[i + 1], 0.0) * size);
  }
}


const std::vector<GLshort> &Mesh::indices_get() const
{
  return indices;
}


void Mesh::positions_add(const vec3 &v)
{
  positions.push_back(v);
}


void Mesh::positions_clear()
{
  positions.clear();
}


const std::vector<vec3> &Mesh::positions_get() const
{
  return positions;
}


void Mesh::positions_update(const mat4 &model)
{
  for (size_t i = 0; i < positions.size(); i++) {
    positions[i] = vec3(model * vec4(positions[i], 1.0));
   // positions[i] = vec3(positions[i].x, positions[i].z, positions[i].y);
  }

  for (size_t i = 0; i < normals.size(); i++) {
    normals[i] = vec3(model * vec4(normals[i], 0.0));
    //normals[i] = vec3(normals[i].x, normals[i].z, normals[i].y);
  }

  for (size_t i = 0; i < tangents.size(); i++) {
    tangents[i] = vec3(model * vec4(tangents[i], 0.0));
  }

  for (size_t i = 0; i < bone_weights.size(); i++) {
    //bone_weights[i] = vec3(bone_weights[i].x, bone_weights[i].z, bone_weights[i].y);
  }

}


void Mesh::print(const int indent_level)
{
  indent(std::cout, indent_level);

  std::cout << "\tvertices: " << num_vertices_get() << ", indices: " << num_indices_get() << std::endl;
}


unsigned int Mesh::num_indices_get() 
{
  return indices.size();
}


unsigned int Mesh::num_vertices_get() 
{
  return positions.size();
}


unsigned int Mesh::num_texture_st_get() 
{
  return texture_st.size();
}


const std::vector<vec3> &Mesh::bitangents_get() const
{
  return bitangents;
}


const std::vector<ivec3> &Mesh::bone_indices_get() const
{
  return bone_indices;
}




const std::vector<vec3> &Mesh::normals_get() const
{
  return normals;
}


const std::vector<vec3> &Mesh::tangents_get() const
{
  return tangents;
}


void Mesh::texture_st_add(const vec2 &v)
{
  texture_st.push_back(v);
}


void Mesh::texture_st_clear()
{
  texture_st.clear();
}


const std::vector<vec2> &Mesh::texture_st_get() const
{
  return texture_st;
}



