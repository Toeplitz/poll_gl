#include "node.h"
#include "utils.h"


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
  tree_level(0) 
{
    state.animated = false;
    state.debug = false;
    state.debug = false;
    state.diffuse = false;
    state.diffuse_normal = false;
    state.diffuse_specular_normal = false;
    state.standard = false;
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


void Node::print_state(int indent_level)
{
  indent(std::cout, indent_level);

  std::cout << "\t\tAnimation: " << state.animated << std::endl;
  std::cout << "\t\tDebug: " << state.debug << std::endl;
  std::cout << "\t\tDiffuse: " << state.diffuse << std::endl;
  std::cout << "\t\tDiffuse normal: " << state.diffuse_normal << std::endl;
  std::cout << "\t\tDiffuse normal specular: " << state.diffuse_specular_normal << std::endl;
  std::cout << "\t\tStandard: " << state.standard << std::endl;

}


void Node::local_transform_current_set(const glm::mat4 &transform) 
{
  transform_local_current = transform;
}


void Node::local_transform_original_set(const glm::mat4 &transform) 
{
  transform_local_original = transform;
}


