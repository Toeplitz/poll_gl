#include "transform.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Transform::Transform()
{
}


Transform::~Transform()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Transform::calculateGlobalTransformTopDown(Node &node)
{
  glm::mat4 transform = node.transform_local_current;
  Node *parent = node.parent;

  if (parent) {
    node.transform_global = parent->transform_global * transform;
  } else {
    node.transform_global = transform;
  }

  for (auto &child : node.children) {
    calculateGlobalTransformTopDown(*child);
  }
}

void Transform::translate(Node &node, glm::vec3 v)
{
  glm::mat4 t = glm::translate(glm::mat4(1.f), v);
  node.transform_local_current = node.transform_local_current * t;
  calculateGlobalTransformTopDown(node);
}
