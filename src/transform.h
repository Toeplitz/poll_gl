#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "node.h"

class Transform 
{
  public:
    Transform();
    ~Transform();


    void calculateGlobalTransformTopDown(Node &node);
    void translate(Node &node, glm::vec3 v);
};

#endif
