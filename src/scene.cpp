#include "assets.h"
#include "scene.h"
#include "glcontext.h"
#include "utils.h"
#include "window.h"
#include <glm/gtx/string_cast.hpp>


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/

Scene::Scene(): 
  root("Fragmic") 
{
  auto transform = mat4(1.f);
  /*
  transform[0][0] = 1;
  transform[1][2] = -1;
  transform[2][1] = 1;
  transform[3][3] = 1;

  POLL_DEBUG(std::cout, glm::to_string(transform));

  glm::mat4 transform = glm::rotate(glm::mat4(1.f), (float) -M_PI /2.f, glm::vec3(1, 0, 0));
*/
  root.transform_global_set(transform);
  root.transform_local_current_set_only(transform);
  root.transform_local_original_set(mat4(1));

}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


Assets &Scene::assets_get() 
{
  return assets;
}


const std::vector <Node *> &Scene::animated_nodes_get() const
{
  return animated_nodes;
}


void Scene::animated_nodes_update_transforms(Node &node, const double dt)
{
  glm::mat4 transform = node.transform_local_current_get();
  Node *parent = node.parent_get();

  if (node.keyframe_total_num_get()) {
    double factor = node.step_time(dt);
    glm::mat4 m = node.keyframe_interpolate(factor);
    node.transform_local_current_set_only(m);
  }

  if (parent) {
    mat4 global_transform;
    global_transform = parent->transform_global_get();
    node.transform_global_set(global_transform * transform);
  } else {
    node.transform_global_set(transform);
  }

  for (auto &child : node.children_get()) {
    animated_nodes_update_transforms(*child, dt);
  }
}


Camera *Scene::camera_get() 
{
  if (!node_cur_camera) {
    std::cout << "Error: no active camera node defined." << std::endl;
    return nullptr;
  }
  if (!node_cur_camera->camera_get()) {
    std::cout << "Error: the activa camera node has no camera defined." << std::endl;
    return nullptr;
  }

  return node_cur_camera->camera_get();
}


GLcontext &Scene::glcontext_get()
{
  return window_ptr->glcontext_get();
}


void Scene::init(Poll_Plugin_List &plugins, Window &window)
{
  this->window_ptr = &window;
  this->plugins_ptr = &plugins;

  Node *cam_node = node_create("camera", &root, TRANSFORM_INHERIT_POSITION_ONLY);
  cam_node->camera_create(assets_get());
  cam_node->camera_get()->transform_perspective_create(window.width_get(), window.height_get());
  node_camera_set(cam_node);
}


Node &Scene::load(const std::string &prefix, const std::string &filename, const unsigned int options) 
{
  GLcontext &glcontext = glcontext_get();
  Model model;
  Node *root_ptr = model.load(*this, root, prefix, filename, options);
  node_state_recursive_update(root);

  transform_update_global_recursive(root_ptr);
  node_recursive_init(glcontext, *root_ptr);

  return *root_ptr;
}


const std::vector<Node *> &Scene::mesh_nodes_get() const
{
  return mesh_nodes;
}


Node *Scene::node_find(Node *root_ptr, const std::string &name)
{
  if (root_ptr) 
    return node_find_recursive(*root_ptr, name);
  
  return node_find_recursive(root, name);
}


void Scene::scene_graph_print(bool compact) 
{
  scene_graph_print_by_node(root, compact);
}


void Scene::scene_graph_print_by_node(Node &node, bool compact)
{
  indent(std::cout, node.tree_level_get());
  std::cout << node.tree_level_get() << ": '" << node.name_get() << "' " << &node << "";

  if (node.armature_get()) {
    std::cout << " (armature)";
  }
  if (node.camera_get()) {
    std::cout << " (camera)";
  }
  if (node.light_get()) {
    std::cout << " (light)";
  }
  if (node.keyframe_total_num_get() > 0) {
    std::cout << " (" << node.keyframe_total_num_get() << " keyframes)";
  }
  if (node.material_get()) {
    std::cout << " (material)";
  }
  if (node.mesh_get()) {
    std::cout << " (mesh)";
  }
  if (node.text_get()) {
    std::cout << " (text)";
  }

  if (!compact) {
    if (node.mesh_get()) {
      node.mesh_get()->print(node.tree_level_get());
    }
    if (node.material_get()) {
      node.material_get()->print(node.tree_level_get());
    }
    if (node.light_get()) {
      node.light_get()->print(node.tree_level_get());
    }
  }
  /*
  std::cout << std::endl;
  std::cout << "global: " << glm::to_string(node.transform_global_get()) << std::endl;
  std::cout << "cur local: " << glm::to_string(node.transform_local_current_get()) << std::endl;
  std::cout << "orig local: " << glm::to_string(node.transform_local_original_get()) << std::endl;
  std::cout << "local scale: " << glm::to_string(node.transform_scale_get()) << std::endl;
  std::cout << "local rotate: " << glm::to_string(node.transform_rotate_get()) << std::endl;
  std::cout << "local translate: " << glm::to_string(node.transform_translate_get()) << std::endl;
  std::cout << "global translate: " << glm::to_string(node.transform_global_translate_get()) << std::endl;

  std::cout << " active: " << node.active_get();
  */
  std::cout << std::endl;

  for (auto &child : node.children_get()) {
    scene_graph_print_by_node(*child, compact);
  }
}


void Scene::node_camera_set(Node *camera_node)
{
  this->node_cur_camera = camera_node;
}


Node *Scene::node_camera_get()
{
  return node_cur_camera;
}


Node *Scene::node_create(const std::string &name, Node *parent, Transform_Inherit transform_inheritance)
{
  std::unique_ptr<Node> node(new Node(name));
  node->transform_inheritance_set(transform_inheritance);
  Node *node_ptr  = node.get();
  if (!parent)
    root.child_add(std::move(node), root.tree_level_get() + 1);
  else 
    parent->child_add(std::move(node), parent->tree_level_get() + 1);

  auto plugins = plugins_get();
  for (auto &plugin : plugins) {
    plugin->cb_node_create(node_ptr);
  }

  transform_update_global_recursive(node_ptr);
  return node_ptr;
}


void Scene::node_positions_update_recursive(Node &node)
{
  auto transform = blender_transform_get();
  Mesh *mesh = node.mesh_get();

  if (mesh) {
    mesh->positions_update(transform);
  }

  for (auto &child : node.children_get()) {
    node_positions_update_recursive(*child);
  }
}


Node &Scene::node_root_get() 
{
  return root;
}


Physics &Scene::physics_get() 
{
  return physics;
}


const Poll_Plugin_List &Scene::plugins_get() const
{
  return *plugins_ptr;
}


void Scene::transform_update_global_recursive(Node *node)
{
  mat4 transform_scale = node->transform_scale_get();
  mat4 transform_translate = node->transform_translate_get();
  mat4 transform_rotate = node->transform_rotate_get();
  Node *parent = node->parent_get();

  if (parent) {
    mat4 global_transform;
    mat4 global_transform_scale;
    mat4 global_transform_translate;
    mat4 global_transform_rotate;

    global_transform_scale = parent->transform_global_scale_get();
    global_transform_translate = parent->transform_global_translate_get();
    global_transform_rotate = parent->transform_global_rotate_get();

    node->transform_global_scale_set(global_transform_scale * transform_scale);
    node->transform_global_translate_set(global_transform_translate * transform_translate);
    node->transform_global_rotate_set(global_transform_rotate * transform_rotate);
  } else {
    node->transform_global_scale_set(transform_scale);
    node->transform_global_translate_set(transform_translate);
    node->transform_global_rotate_set(transform_rotate);
  }

  node->transform_full_update();

  for (auto &child : node->children_get()) {
    transform_update_global_recursive(child.get());
  }
}


Window &Scene::window_get()
{
  return *window_ptr;
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


void Scene::animated_nodes_add(Node &node) 
{
  bool r = (std::find(animated_nodes.begin(), animated_nodes.end(), &node) != animated_nodes.end());
  if (r) {
    std::cout << "Node is already in transform queue." << std::endl;
    return;
  }
  animated_nodes.push_back(&node);
}



void Scene::mesh_nodes_add(Node &node) 
{
  mesh_nodes.push_back(&node);
}


Node *Scene::node_find_recursive(Node &node, const std::string &name)
{
  Node *ret = nullptr;
  if (!node.name_get().compare(name)) {
    return &node;
  }
  for (auto &child : node.children_get()) {
    ret = node_find_recursive(*child, name);
    if (ret)
      return ret;
  }

  return ret;
}


void Scene::node_recursive_init(GLcontext &glcontext, Node &node) 
{
  Armature *armature = node.armature_get();
  Mesh *mesh = node.mesh_get();
  Material *material = node.material_get();

  if (armature) {
    animated_nodes_add(node);
  }

  if (mesh) {
    if (!(node.import_options & MODEL_IMPORT_NO_DRAW)) {
      mesh_nodes_add(node);
    }
    if (!(node.import_options & MODEL_IMPORT_NO_UPLOAD)) {

      glcontext.vertex_buffers_mesh_create(mesh);
    }
  } 


  if (material) {
    if (!(node.import_options & MODEL_IMPORT_NO_UPLOAD)) {
      glcontext.texture_materials_create(material);
    }
  }
  
  node_state_recursive_update(node);

  for (auto &child : node.children_get()) {
    node_recursive_init(glcontext, *child);
  }
}


void Scene::node_state_recursive_update(Node &node)
{
  Node_State &state = node.state_get();

  if (node.armature_get()) {
    state.animated = true;
  }

  if (node.material_get()) {
    if (node.material_get()->diffuse && node.material_get()->normal && node.material_get()->specular ) {
      state.diffuse_specular_normal = true;
    } else if (node.material_get()->diffuse && node.material_get()->normal) {
      state.diffuse_normal = true;
    } else if (node.material_get()->diffuse) {
      state.diffuse = true;
    } else if (node.material_get()->cubemap) {
      std::cout << "Updating material state" << std::endl;
      if (node.material_get()->cubemap->type == CUBEMAP_SKYBOX) {
        state.cubemap_skybox = true;
        std::cout << "Found skybox" << std::endl;
      } else if (node.material_get()->cubemap->type == CUBEMAP_REFLECTION) {
        std::cout << "Found reflection" << std::endl;
        state.cubemap_reflect = true;
      } else {
        std::cout << "Error: cubemap type not recognized" << std::endl;
      }
      state.standard = false;
    } else {
      state.standard = true;
    }
  }

  for (auto &child : node.children_get()) {
    node_state_recursive_update(*child);
  }
}

