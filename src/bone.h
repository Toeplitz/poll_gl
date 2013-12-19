#ifndef BONE_H
#define BONE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "node.h"

class Bone {

  private:
    glm::mat4 skinningMatrix;
    unsigned int id;

  public:
    std::string name;
    const glm::mat4 offsetMatrix;
    Node *jointNode;

    Bone(const std::string & _name, const unsigned int &_id,
        const glm::mat4 & _offsetMatrix, Node * _jointNode);
    ~Bone();

    const unsigned int getIndex();
    glm::mat4 updateSkinningMatrix();


};


#endif
