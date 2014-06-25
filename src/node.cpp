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
  transform_global(1),
  transform_local_current(1),
  transform_local_original(1), 
  tree_level(0) {
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
  node->tree_level = level;
  children.push_back(std::move(node));
}

void Node::local_transform_current_set(const glm::mat4 &transform) 
{
  transform_local_current = transform;
}


void Node::local_transform_original_set(const glm::mat4 &transform) 
{
  transform_local_original = transform;
}


