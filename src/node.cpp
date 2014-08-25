#include "assets.h"
#include "node.h"
#include "utils.h"
#include <glm/gtx/string_cast.hpp>


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Node::Node(const std::string &node_name):
  name(node_name),
  transform_global(1),
  transform_local_current(1),
  transform_local_original(1)
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

Armature *Node::armature_get()
{
  return armature;
}

void Node::armature_set(Armature *armature)
{
  this->armature = armature;
}


Camera *Node::camera_create(Assets &assets)
{
  std::unique_ptr<Camera> camera(new Camera());
  Camera *camera_ptr = camera.get();
  camera_set(camera_ptr);
  assets.camera_add(std::move(camera));
  return camera_ptr;
}


Camera *Node::camera_get()
{
  return camera;
}


void Node::camera_set(Camera *camera)
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
  mesh_set(assets.light_sphere_get()->mesh_get());
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
  if (light)
    light->node_ptr_set(this);


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


Manipulator *Node::manipulator_create(Assets &assets)
{
  std::unique_ptr<Manipulator> manipulator(new Manipulator());
  Manipulator *manipulator_ptr = manipulator.get();
  manipulator_set(manipulator_ptr);
  assets.manipulator_add(std::move(manipulator));
  return manipulator_ptr;
}


Manipulator *Node::manipulator_get()
{
  return manipulator;
}


void Node::manipulator_set(Manipulator *manipulator)
{
  this->manipulator= manipulator;
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
  glm::mat4 m = glm::scale(transform_model_get(), v);
  transform_model_set(m);
}


Text *Node::text_create(Font *font, Assets &assets)
{
  std::unique_ptr<Text> text(new Text());
  Text *text_ptr = text.get();
  text_ptr->font_set(font);
  text_set(text_ptr);

  if (!mesh_get())
    mesh_create(assets);

  assets.text_add(std::move(text));
  return text_ptr;
}


Text *Node::text_get()
{
  return text;
}


void Node::text_set(Text *text)
{
  this->text = text;
}


void Node::translate(const glm::vec3 &v) 
{
  glm::mat4 m = glm::translate(transform_model_get(), v);
  transform_model_set(m);
}


void Node::transform_local_current_set(const glm::mat4 &transform) 
{
  transform_local_current = transform;
}


void Node::transform_local_original_set(const glm::mat4 &transform) 
{
  transform_local_original = transform;
}


glm::mat4 &Node::transform_model_get()
{
  return transform_model; 
}


void Node::transform_model_set(const glm::mat4 &transform)
{
  transform_model = transform;
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

  for (auto &child : node.children) {
    transform_update_global_recursive(*child);
  }
}
