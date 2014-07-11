#pragma once

#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "bone.h"
#include "node.h"


struct Animation
{
  std::string name;
  unsigned int keyframe_first;
  unsigned int keyframe_last;
};


typedef std::vector<std::unique_ptr<Bone>> Bone_Unique_Ptr_List;


class Armature {
  private:
    std::map<std::string, Animation> animations;
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
    void                        keyframe_range_activate(const std::string &name);
    void                        keyframe_range_set(const std::string &name, const unsigned int keyframe_first, const unsigned int keyframe_last);
    void                        node_print_all();
};


