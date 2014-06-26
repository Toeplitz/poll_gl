#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "node.h"

class Bone {
  private:
    unsigned int     id;
    std::string      name;
    const glm::mat4  offset_matrix;

  public:
    Node            *joint_node;

    Bone(const std::string &name, const unsigned int &id, const glm::mat4 &m, Node *joint_node);
    ~Bone();

    unsigned int  index_get();
    glm::mat4     skinning_matrix_update();
};
