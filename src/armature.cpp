#include "armature.h"


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Armature::bones_add(std::unique_ptr<Bone> &&bone) {
  assert(bones.size() == bone->index_get());
  bones.push_back(std::move(bone));
  skinning_matrices.resize(bones.size());
}


Bone_Unique_Ptr_List const &Armature::bones_get_all() const
{
  return bones;
} 


unsigned int Armature::bones_num_get()
{
  return bones.size();
}


void Armature::bones_update_skinningmatrices()
{
  for (auto &bone_entry: bones) {
    Bone &bone = *bone_entry;
    const unsigned int id = bone.index_get();
    skinning_matrices[id] = bone.skinning_matrix_update();
  }
}


Node *Armature::find_toplevel_node()
{
  Node *root = nullptr;
  for (auto &boneEntry : bones) {
    Bone &bone = *boneEntry;
    if (!root) {
      root = bone.joint_node;
    } else {
      Node *node = bone.joint_node;
      if (root->tree_level_get() >= node->tree_level_get()) {
        root = node;
      }
    }
  }
  if (!root->parent_get())
    return root;

  return root->parent_get();
}


void Armature::keyframe_range_activate(const std::string &name)
{
  static Animation *a_last;

  if (animations.find(name) == animations.end()) {
    std::cout << "Error: could not find requested keyframe animation! Needs to be added?" << std::endl;
    return;
  }

  Animation &a = animations[name];
  if (a_last == &a)
    return;

  for (auto &bone_entry : bones) {
    Bone &bone = *bone_entry;
    bone.joint_node->animation_activate(a);
  }

  a_last = &a;
}


void Armature::keyframe_range_set(const std::string &name, const unsigned int keyframe_first, const unsigned int keyframe_last)
{
  Animation a;

  a.keyframe_first = keyframe_first;
  a.keyframe_last = keyframe_last;
  a.name = name;
  animations[name] = a;
}



void Armature::node_print_all()
{
  std::cout << "Nodes in armature: " << std::endl;
  for (auto &bone_entry : bones) {
    Bone &bone = *bone_entry;
    std::cout << bone.joint_node->name_get() << std::endl;
  }
}
