#include "assets.h"
#include "node.h"
#include "utils.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Node::Node(const std::string &node_name):
  camera(nullptr),
  light(nullptr),
  rigidbody(nullptr),
  armature(nullptr),
  material(nullptr),
  mesh(nullptr),
  parent(nullptr),
  name(node_name),
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


Camera_Proto *Node::camera_create(Assets &assets)
{
  std::unique_ptr<Camera_Proto> camera(new Camera_Proto());
  Camera_Proto *camera_ptr = camera.get();
  camera_set(camera_ptr);
  assets.camera_add(std::move(camera));
  return camera_ptr;
}


Camera_Proto *Node::camera_get()
{
  return camera;
}


void Node::camera_set(Camera_Proto *camera)
{
  this->camera = camera;
}


void Node::copy_transform_data(Node &node)
{
  original_rotation = node.original_rotation;
  original_scaling = node.original_scaling;
  original_position = node.original_position;
  transform_update_global_recursive(*this);
}


void Node::child_add(std::unique_ptr<Node> &&node, int level) 
{
  node->parent = this;
  node->tree_level = level;
  children.push_back(std::move(node));
}


Light *Node::light_create(Assets &assets)
{
  std::unique_ptr<Light> light(new Light());
  Light *light_ptr = light.get();
  light_set(light_ptr);
  assets.light_active_add(std::move(light));
  return light_ptr;
}


Light *Node::light_get()
{
  return light;
}


void Node::light_set(Light *light)
{
  if (mesh) {
    light->properties_position_set(glm::vec3(mesh->model * glm::vec4(original_position, 1.0)));
  }
  this->light = light;
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


Physics_Rigidbody  *Node::physics_rigidbody_create(Assets &assets)
{
  std::unique_ptr<Physics_Rigidbody> rigidbody(new Physics_Rigidbody());
  Physics_Rigidbody *rigidbody_ptr = rigidbody.get();
  physics_rigidbody_set(rigidbody_ptr);
  assets.physics_rigidbody_add(std::move(rigidbody));
  return rigidbody_ptr;
}


Physics_Rigidbody  *Node::physics_rigidbody_get()
{
  return rigidbody;
}


void Node::physics_rigidbody_set(Physics_Rigidbody *rigidbody)
{
  this->rigidbody = rigidbody;
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


void Node::rotate(const float angle, const glm::vec3 &v)
{
  glm::mat4 m = glm::rotate(glm::mat4(1.f), angle, v);
  transform_local_current = transform_local_current * m;
  transform_update_global_recursive(*this);
}


void Node::scale(const glm::vec3 &v)
{
  glm::mat4 m = glm::scale(glm::mat4(1.f), v);
  transform_local_current = transform_local_current * m;
  transform_update_global_recursive(*this);
}


void Node::translate(const glm::vec3 &v)
{
  glm::mat4 m = glm::translate(glm::mat4(1.f), v);
  transform_local_current = transform_local_current * m;
  transform_update_global_recursive(*this);
}


void Node::transform_local_current_set(const glm::mat4 &transform) 
{
  transform_local_current = transform;
}


void Node::transform_local_original_set(const glm::mat4 &transform) 
{
  transform_local_original = transform;
}


void Node::transform_update_global_recursive(Node &node)
{
  glm::mat4 transform = node.transform_local_current;
  Node *parent = node.parent;

  if (parent) {
    node.transform_global = parent->transform_global * transform;
  } else {
    node.transform_global = transform;
  }

 // if (node.mesh && !node.armature) {
 //   node.mesh->model = node.transform_global;;
 // }

  for (auto &child : node.children) {
    transform_update_global_recursive(*child);
  }
}
