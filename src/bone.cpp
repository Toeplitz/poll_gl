#include "bone.h"
#include "utils.h"
#include "model.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Bone::Bone(const std::string &name, const unsigned int &id, glm::mat4 &m, Node *joint_node, const unsigned int options)
{
  this->id = id;
  this->name = name;
  this->joint_node = joint_node;


  if (options & MODEL_IMPORT_BLENDER_FIX) {
    this->offset_matrix = m * glm::inverse(blender_transform_get());
  } else {
    this->offset_matrix = m;
  }
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
