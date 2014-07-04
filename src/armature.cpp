#include "armature.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Armature::Armature()
{
} 


Armature::~Armature() 
{
}


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
      if (root->tree_level >= node->tree_level) {
        root = node;
      }
    }
  }
  if (!root->parent)
    return root;

  return root->parent;
}
