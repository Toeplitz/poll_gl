#include "bone.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Bone::Bone(const std::string &name, const unsigned int &id, const glm::mat4 &m, Node *joint_node):
  offset_matrix(m)
{
  this->id = id;
  this->name = name;
  this->joint_node = joint_node;
} 


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


unsigned int Bone::index_get()
{
  return id;
}


glm::mat4 Bone::skinning_matrix_update() 
{
  return joint_node->transform_global_get() * offset_matrix;
}
