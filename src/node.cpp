#include "assets.h"
#include "node.h"
#include "utils.h"
#include "transform.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Node::Node(const std::string &_name):
  armature(nullptr),
  light(nullptr),
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
  state.cubemap_reflect = false;
  state.cubemap_skybox = false;
  state.standard = false;
} 


Node::~Node() 
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Node::armature_set(Armature *armature)
{
  this->armature = armature;
}


void Node::copy_transform_data(Node &node)
{
  Transform t;
  original_rotation = node.original_rotation;
  original_scaling = node.original_scaling;
  original_position = node.original_position;
  t.calculateGlobalTransformTopDown(node);
}


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


void Node::print_state(int indent_level)
{
  indent(std::cout, indent_level);

  std::cout << "\t\tAnimation: " << state.animated << std::endl;
  std::cout << "\t\tDebug: " << state.debug << std::endl;
  std::cout << "\t\tDiffuse: " << state.diffuse << std::endl;
  std::cout << "\t\tDiffuse normal: " << state.diffuse_normal << std::endl;
  std::cout << "\t\tDiffuse normal specular: " << state.diffuse_specular_normal << std::endl;
  std::cout << "\t\tCubemap reflect: " << state.cubemap_reflect << std::endl;
  std::cout << "\t\tCubemap skybox: " << state.cubemap_skybox << std::endl;
  std::cout << "\t\tStandard: " << state.standard << std::endl;
}

Mesh *Node::mesh_create(Assets &assets)
{
  std::unique_ptr<Mesh> mesh(new Mesh());
  Mesh *mesh_ptr = mesh.get();
  mesh_set(mesh_ptr);
  assets.mesh_add(std::move(mesh));
  return mesh_ptr;
}


Mesh *Node::mesh_get()
{
  if (!mesh) {
    std::cout << "Node: '" << name << "' does not have any mesh attached" << std::endl;
  }

  return mesh;
}


void Node::mesh_set(Mesh *mesh)
{
  this->mesh = mesh;
}


Material *Node::material_create(Assets &assets)
{
  std::unique_ptr<Material> material(new Material());
  Material *material_ptr = material.get();
  material_set(material_ptr);
  assets.material_add(std::move(material));
  return material_ptr;
}


Material *Node::material_get()
{
  if (!material) {
    std::cout << "Node: '" << name << "' does not have any material attached" << std::endl;
  }

  return material;
}


void Node::material_set(Material *material)
{
  this->material = material;
}


Light *Node::light_create(Assets &assets)
{
  std::unique_ptr<Light> light(new Light());
  Light *light_ptr = light.get();
  light_set(light_ptr);
  assets.light_add(std::move(light));
  return light_ptr;
}


Light *Node::light_get()
{
  if (!light) {
    std::cout << "Node: '" << name << "' does not have any light attached" << std::endl;
  }

  return light;
}


void Node::light_set(Light *light)
{
  if (mesh) {
    light->properties_position_set(glm::vec3(mesh->model * glm::vec4(original_position, 1.0)));
  }
  this->light = light;
}
