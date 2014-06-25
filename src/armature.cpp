#include "armature.h"

Armature::Armature()
{
} 


Armature::~Armature() 
{
}


unsigned int Armature::num_bones()
{
  return bones.size();
}


Node *Armature::findArmatureRootNode()
{
  Node *root = nullptr;

  for (auto &boneEntry : bones) {
    Bone &bone = *boneEntry;

    if (!root) {
      root = bone.jointNode;
    } else {
      Node *node = bone.jointNode;

      if (root->treeLevel >= node->treeLevel) {
        root = node;
      }

    }
  }

  if (!root->parent)
    return root;

  return root->parent;
}


void Armature::update_bones()
{
  for (auto &bone_entry: bones) {
    Bone &bone = *bone_entry;
    const unsigned int id = bone.get_index();
    skinning_matrices[id] = bone.updateSkinningMatrix();
  }
}


void Armature::addBone(std::unique_ptr<Bone> &&bone) {
  assert(bones.size() == bone->get_index());
  bones.push_back(std::move(bone));
  skinning_matrices.resize(bones.size());
}

