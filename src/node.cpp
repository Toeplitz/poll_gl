#include "node.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Node::Node(const std::string &_name):
  armature(nullptr),
  material(nullptr),
  mesh(nullptr),
  parent(nullptr),
  name(_name),
  currentLocalTransform(1),
  originalLocalTransform(1), 
  treeLevel(0) {
  } 


Node::~Node() 
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Node::child_add(std::unique_ptr<Node> &&node, int level) 
{
  node->parent = this;
  node->treeLevel = level;
  children.push_back(std::move(node));
}

void Node::local_transform_current_set(const glm::mat4 &transform) 
{
  currentLocalTransform = transform;
}


void Node::local_transform_original_set(const glm::mat4 &transform) 
{
  originalLocalTransform = transform;
}


