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
    glm::mat4  offset_matrix;

  public:
    Node            *joint_node;

    Bone(const std::string &name, const unsigned int &id, glm::mat4 &m, Node *joint_node);

    unsigned int  index_get();
    glm::mat4     skinning_matrix_update();
};
