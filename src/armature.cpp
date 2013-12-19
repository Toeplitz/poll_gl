#include "armature.h"

Armature::Armature()
{
} 


Armature::~Armature() 
{
}


unsigned int Armature::getNumBones()
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


void Armature::updateBones()
{
  for (auto &boneEntry : bones) {
    Bone &bone = *boneEntry;
    const unsigned int id = bone.getIndex();
    skinningMatrices[id] = bone.updateSkinningMatrix();
  }
}


void Armature::update(GLuniformBuffer *buffer)
{
  buffer->update(skinningMatrices, 0);
}


void Armature::addBone(std::unique_ptr<Bone> &&bone) {
  assert(bones.size() == bone->getIndex());
  bones.push_back(std::move(bone));
  skinningMatrices.resize(bones.size());
}

