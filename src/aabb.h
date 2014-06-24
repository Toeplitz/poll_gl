#ifndef AABB_H
#define AABB_H

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
    glm::vec3 center;
    glm::vec3 size;
 
  public:
    std::vector<Vertex> vertices;
    std::vector<GLshort> indices;
    glm::mat4 transform;


    Aabb();
    ~Aabb();

    const unsigned int getNumVertices() const;
    const unsigned int getNumIndices() const;

    void addVertex(const float x, const float y, const float z);
    void addIndex(GLshort index);

    void setBounds(float minX, float minY, float minZ,
                   float maxX, float maxY, float maxZ);

};





#endif
