#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "bone.h"
#include "node.h"

class Armature {
  private:
    std::vector<std::unique_ptr<Bone>> bones;

  public:
    std::vector<glm::mat4> skinning_matrices;

    Armature();
    ~Armature();

    void          bones_add(std::unique_ptr<Bone> &&bone);
    void          bones_update_skinningmatrices();
    unsigned int  bones_num_get();
    Node         *find_toplevel_node();
};


