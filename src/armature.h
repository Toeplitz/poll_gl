#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <vector>

#include "bone.h"
#include "node.h"
#include "gldefaults.h"

class Armature {
  private:
    std::vector<std::unique_ptr<Bone>> bones;

  public:
    Armature();
    ~Armature();
    std::vector<glm::mat4> skinning_matrices;

    void          addBone(std::unique_ptr<Bone> &&bone);
    unsigned int  num_bones();
    Node         *findArmatureRootNode();
    void          update_bones();

};


