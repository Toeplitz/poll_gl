#include "bone.h"


Bone::Bone(const std::string & _name, const unsigned int &_id,
    const glm::mat4 & _offsetMatrix, Node * _jointNode):
  skinningMatrix(1), 
  id(_id),
  name(_name),
  offsetMatrix(_offsetMatrix),
  jointNode(_jointNode) 
{
} 


Bone::~Bone()
{
}


unsigned int Bone::get_index()
{
  return id;
}


glm::mat4 Bone::updateSkinningMatrix() 
{
  skinningMatrix = jointNode->globalTransform * offsetMatrix;

  return skinningMatrix;
}
