#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "bone.h"
#include "node.h"


typedef std::vector<std::unique_ptr<Bone>> Bone_Unique_Ptr_List;


class Armature {
  private:
    Bone_Unique_Ptr_List bones;

  public:
    std::vector<glm::mat4> skinning_matrices;

    Armature();
    ~Armature();

    void                        bones_add(std::unique_ptr<Bone> &&bone);
    Bone_Unique_Ptr_List const &bones_get_all() const;
    unsigned int                bones_num_get();
    void                        bones_update_skinningmatrices();
    Node                       *find_toplevel_node();
};


