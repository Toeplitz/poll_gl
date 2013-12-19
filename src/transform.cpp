#include "transform.h"

Transform::Transform()
{
}


Transform::~Transform()
{
}


void Transform::calculateGlobalTransformTopDownConcurrent(std::vector<Node *> nodes)
{

}


void Transform::calculateGlobalTransformTopDown(Node &node)
{
  glm::mat4 transform = node.currentLocalTransform;
  Node *parent = node.parent;

  if (parent) {
    node.globalTransform = parent->globalTransform * transform;
  } else {
    node.globalTransform = transform;
  }

  for (auto &child : node.children) {
    calculateGlobalTransformTopDown(*child);
  }
}


void Transform::calculateGlobalTransform(Node &node)
{
  glm::mat4 transform = node.currentLocalTransform;
  Node *nodePtr = node.parent;

  while (nodePtr) {
    transform = nodePtr->currentLocalTransform * transform;
    nodePtr = nodePtr->parent;
  }

  node.globalTransform = transform;
}


void Transform::translate(Node &node, glm::vec3 v)
{
  glm::mat4 t = glm::translate(glm::mat4(1.f), v);
  node.currentLocalTransform = node.currentLocalTransform * t;
  calculateGlobalTransformTopDown(node);
}
