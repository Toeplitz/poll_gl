#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "gldefaults.h"
#include "vertex.h"


class Aabb
{
  private:
 
  public:
    glm::vec3 center;
    glm::vec3 size;
    std::vector<Vertex> vertices;
    std::vector<GLshort> indices;
    glm::mat4 transform;


    Aabb();
    ~Aabb();

    void addVertex(const float x, const float y, const float z);
    void addIndex(GLshort index);

    void setBounds(float minX, float minY, float minZ,
                   float maxX, float maxY, float maxZ);

};

