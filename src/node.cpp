#include "scene.h"
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


void  Node::active_set(Scene &scene, const bool flag)
{
  this->active = flag;

  Physics_Rigidbody *rigidbody = physics_rigidbody_get();
  if (rigidbody) {
    if (flag) {
      scene.physics_get().rigidbody_add(rigidbody);
    } else {
      scene.physics_get().rigidbody_delete(rigidbody);
    }
  }

  for (auto &child : children_get()) {
    child->active_set(scene, flag);
  }
  
}


bool Node::active_get()
{
  return active;
}


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

void Node::current_scale_set(glm::vec3 &v)
{
  transform_scale = glm::scale(mat4(1.f), v);
  this->current_scale = v;
}


glm::vec3 &Node::current_scale_get()
{
  return current_scale;
}


void Node::current_translate_set(glm::vec3 &v)
{
  transform_translate = glm::translate(mat4(1.f), v / current_scale);
  this->current_translate = v;
}


glm::vec3 &Node::current_translate_get()
{
  return current_translate;
}


Light *Node::light_create(Scene &scene, const unsigned int lamp_type, const unsigned int illumination_type)
{
  Assets &assets = scene.assets_get();
  Stock_Nodes &stock_nodes = assets.stock_nodes_get();
  Node *node_ptr = nullptr;

  if (illumination_type == Light::GLOBAL) {
    node_ptr = stock_nodes.screen_quad_get();
  } else {
    node_ptr = stock_nodes.sphere_get();
  }

  if (!node_ptr) {
    std::cout << "Error: Light type was not set" << std::endl;
    return nullptr;
  }

  std::unique_ptr<Light> light(new Light());
  Light *light_ptr = light.get();
  light_ptr->properties_type_set(lamp_type);
  light_ptr->illumination_type_set(illumination_type);
  mesh_set(node_ptr->mesh_get());
  light_set(light_ptr);

  auto plugins = scene.plugins_get();
  for (auto &plugin : plugins) {
    plugin->light_callback_create(this);
  }

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


Node *Node::parent_get()
{
  return parent;
}


Physics_Rigidbody *Node::physics_rigidbody_create(Scene &scene, bool recursive)
{
  Assets &assets = scene.assets_get();
  Physics_Rigidbody *rigidbody_ptr = nullptr;

  if (!mesh_get()) {
    POLL_ERROR(std::cerr, "Cannot create rigidbody, no mesh for node: " << name_get());
  } else {
    std::unique_ptr<Physics_Rigidbody> rigidbody(new Physics_Rigidbody());
    rigidbody_ptr = rigidbody.get();

    //rigidbody_ptr->create(this, shape, type, initial_mass);
    physics_rigidbody_set(rigidbody_ptr);
 //   scene.physics_get().rigidbody_add(rigidbody_ptr);
    assets.physics_rigidbody_add(std::move(rigidbody));

    /* In case we have done scaling of the node before adding the rigidbody */
    //transform_update_global_recursive(root_ptr);
    ///rigidbody_ptr->motionstate_transform_set(transform_global_get());
  }

  if (recursive) {
    for (auto &child : children_get()) {
      child->physics_rigidbody_create(scene, recursive);
    }
  }

  return rigidbody_ptr;
}


Physics_Rigidbody *Node::physics_rigidbody_get()
{
  return rigidbody;
}


void Node::physics_rigidbody_set(Physics_Rigidbody *rigidbody)
{
  this->rigidbody = rigidbody;
}


const std::string  &Node::name_get()
{
  return name;
}


void Node::name_set(const std::string &name)
{
  this->name = name;
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


Mesh *Node::mesh_create(Scene &scene)
{
  Assets &assets = scene.assets_get();

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


void Node::raycast_collide_callback_call(vec3 &position)
{
  if (!raycast_collide_callback)
    return;

  raycast_collide_callback(*this, position);

}

void Node::raycast_collide_callback_set(const std::function <void (Node &node, vec3 &position)> callback)
{
  this->raycast_collide_callback = callback;
}


void Node::rotate(Scene &scene, const float angle, const vec3 &v)
{
 // mat4 m = transform_rotate = glm::rotate(transform_local_current_get(), angle, v);
  transform_rotate = glm::rotate(transform_rotate, angle, v);
  mat4 m = transform_full_update();
  transform_local_current_set(scene, m);
}


void Node::scale(Scene &scene, const vec3 &v)
{
  //mat4 s = glm::scale(transform_local_current_get(), v);
  transform_scale = glm::scale(transform_scale, v);
  current_scale = current_scale * v;

  mat4 m = transform_full_update();
  transform_local_current_set(scene, m);

}


Node_State &Node::state_get()
{
  return state;
}


Text *Node::text_create(Font *font, Scene &scene)
{
  Assets &assets = scene.assets_get();
  std::unique_ptr<Text> text(new Text());
  Text *text_ptr = text.get();
  text_ptr->font_set(font);
  text_set(text_ptr);

  if (!mesh_get())
    mesh_create(scene);

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


void Node::translate(Scene &scene, const vec3 &v) 
{
  Physics_Rigidbody *rigidbody = physics_rigidbody_get();
  //mat4 m = glm::translate(transform_local_current_get(), v / current_scale);
  transform_translate = glm::translate(transform_translate, v);
  current_translate = current_translate + v;

  mat4 m = transform_full_update();

  //POLL_DEBUG(std::cout, glm::to_string(m));
  transform_local_current_set(scene, m);
  if (rigidbody) {
    mat4 &model = transform_global_get();
    POLL_DEBUG(std::cout, glm::to_string(model));
    rigidbody->motionstate_transform_set(model);
  }
}


void Node::transform_inheritance_set(Transform_Inherit transform_inheritance)
{
  this->transform_inheritance = transform_inheritance;
}


Transform_Inherit Node::transform_inheritance_get()
{
  return transform_inheritance;
}

mat4 &Node::transform_global_get()
{
  return transform_global;
}


glm::mat4 Node::transform_full_update()
{
  //return transform_scale * transform_rotate * transform_translate;
  return transform_translate * transform_rotate * transform_scale;
}


mat4 &Node::transform_global_position_get()
{
  mat4 &model = transform_global;
  transform_position_current = glm::translate(mat4(1.f), vec3(model[3][0], model[3][1], model[3][2]));
  return transform_position_current;
}


void Node::transform_global_set(const mat4 &transform)
{
  Physics_Rigidbody *rigidbody = physics_rigidbody_get();

  this->transform_global = transform;

  if (rigidbody) {
    //rigidbody->motionstate_transform_set(transform);
  } else {
   // POLL_DEBUG(std::cout, "No rigidbody for: " << name_get());
  }
}


void Node::transform_local_current_set_only(const mat4 &transform)
{
  this->transform_local_current = transform;
}


void Node::transform_local_current_set(Scene &scene, const mat4 &transform) 
{
  this->transform_local_current = transform;
  scene.transform_update_global_recursive(this);
}


mat4 &Node::transform_local_current_get()
{
  return transform_local_current;
}


void Node::transform_local_original_set(const mat4 &transform) 
{
  this->transform_local_original = transform;
}


mat4 &Node::transform_local_original_get()
{
  return transform_local_original;
}


mat4 Node::transform_rotate_get()
{
  return transform_rotate;
}


mat4 Node::transform_scale_get()
{
  return transform_scale;
}


mat4 Node::transform_translate_get()
{
  return transform_translate;
}


const int &Node::tree_level_get()
{
  return tree_level;
}


void Node::tree_level_set(const unsigned int &tree_level)
{
  this->tree_level = tree_level;
}
