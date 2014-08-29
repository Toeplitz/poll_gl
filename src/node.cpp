#include "assets.h"
#include "node.h"
#include "utils.h"
#include <glm/gtx/string_cast.hpp>


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Node::Node(const std::string &node_name)
{
  name_set(node_name);
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


Node_List const &Node::children_get() const
{
  return children;
}


void Node::child_add(std::unique_ptr<Node> &&node, int level) 
{
  node->parent = this;
  node->tree_level = level;
  children.push_back(std::move(node));
}


Light *Node::light_create(Assets &assets, const unsigned int lamp_type, const unsigned int illumination_type)
{
  Stock_Nodes &stock_nodes = assets.stock_nodes_get();
  Mesh *mesh_symbol_cone = stock_nodes.cone_get();
  Mesh *mesh = nullptr;

  if (illumination_type == Light::GLOBAL) {
    mesh = stock_nodes.screen_quad_get();
  } else {
    mesh = stock_nodes.sphere_get();
  }

  if (!mesh) {
    std::cout << "Error: Light type was not set" << std::endl;
    return nullptr;
  }

  std::unique_ptr<Light> light(new Light());
  Light *light_ptr = light.get();
  light_ptr->mesh_symbol_set(mesh_symbol_cone);
  light_ptr->properties_type_set(lamp_type);
  light_ptr->illumination_type_set(illumination_type);
  mesh_set(mesh);
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


const vec3  &Node::original_position_get()
{
  return original_position;
}


void Node::original_position_set(const vec3 &v)
{
  this->original_position = v;
}


const quat &Node::original_rotation_get()
{
  return original_rotation;
}


void  Node::original_rotation_set(const quat &q)
{
  this->original_rotation = q;
}


const vec3 &Node::original_scaling_get()
{
  return original_scaling;
}


void Node::original_scaling_set(const vec3 &v)
{
  this->original_scaling = v;
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


Node *Node::parent_get()
{
  return parent;
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


const std::string  &Node::name_get()
{
  return name;
}


void Node::name_set(const std::string &name)
{
  this->name = name;
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


void Node::rotate(const float angle, const vec3 &v)
{
  mat4 m = glm::rotate(transform_model_get(), angle, v);
  transform_model_set(m);
}


void Node::scale(const vec3 &v)
{
  mat4 m = glm::scale(transform_model_get(), v);
  transform_model_set(m);
}


Node_State &Node::state_get()
{
  return state;
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


void Node::translate(const vec3 &v) 
{
  mat4 m = glm::translate(transform_model_get(), v);
  transform_model_set(m);
}


void Node::transform_local_current_set(const mat4 &transform) 
{
  transform_local_current = transform;
}


void Node::transform_local_original_set(const mat4 &transform) 
{
  transform_local_original = transform;
}


const mat4 &Node::transform_global_get()
{
  return transform_global;
}


void Node::transform_global_set(const mat4 &transform)
{
  this->transform_global = transform;
}


const mat4 &Node::transform_local_current_get()
{
  return transform_local_current;
}


mat4 &Node::transform_model_get()
{
  return transform_model; 
}


void Node::transform_model_set(const mat4 &transform)
{
  transform_model = transform;
}



void Node::transform_update_global_recursive(Node &node)
{
  mat4 transform = node.transform_local_current;
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


const int &Node::tree_level_get()
{
  return tree_level;
}


void Node::tree_level_set(const unsigned int &tree_level)
{
  this->tree_level = tree_level;
}
