#include "scene.h"
#include "transform.h"
#include "utils.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/

Scene::Scene(): 
  assets(), 
  root(std::string("Fragmic")) 
{
}


Scene::~Scene() 
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


Assets &Scene::assets_get()
{
  return assets;
}


void Scene::animation_list_add(Node &node) 
{
  bool r = (std::find(animation_list.begin(), animation_list.end(), &node) != animation_list.end());
  if (r) {
    std::cout << "Node is already in transform queue." << std::endl;
    return;
  }
  animation_list.push_back(&node);
}


std::vector <Node *> Scene::animation_list_get() 
{
  return animation_list;
}


void Scene::animation_list_update_transforms(Node &node, Uint32 dt)
{
  glm::mat4 transform = node.transform_local_current;
  Node *parent = node.parent;

  if (node.keyframe_total_num_get() != 0) {
    node.step_time((double) dt / 1000.0);
    node.local_transform_current_set(node.transform_local_interpolated);
  }

  if (parent) {
    node.transform_global = parent->transform_global * transform;
  } else {
    node.transform_global = transform;
  }

  for (auto &child : node.children) {
    animation_list_update_transforms(*child, dt);
  }
}



Node &Scene::load_model(const std::string &prefix, const std::string &filename, bool draw) 
{
  Transform transform;

  Model model;
  Node *root_ptr = model.load(assets, root, prefix, filename);
  state_update_recursive(*root_ptr);

  transform.calculateGlobalTransformTopDown(root);

  if (draw) {
    upload_queue_add(*root_ptr);
  }

  return *root_ptr;
}


Node *Scene::node_find(Node *root_ptr, const std::string &name)
{
  if (root_ptr) 
    return node_find_recursive(*root_ptr, name);
  
  return node_find_recursive(root, name);
}


void Scene::render_list_add(Node &node) 
{
  render_list.push_back(&node);
}


std::vector <Node *> Scene::render_list_get() 
{
  return render_list;
}


void Scene::scene_graph_print() 
{
  scene_graph_print_by_node(root);
}


void Scene::scene_graph_print_by_node(Node &node)
{
  indent(std::cout, node.tree_level);
  std::cout << node.tree_level << ": '" << node.name << "' " << &node << "";

  if (node.mesh) {
    std::cout << " (mesh)";
  }
  if (node.armature) {
    std::cout << " (armature)";
  }

  if (node.keyframe_total_num_get() > 0) {
    std::cout << " (" << node.keyframe_total_num_get() << " keyframes)";
  }
  if (node.material) {
    std::cout << " (material)" << std::endl;
  }
  if (node.mesh) {
    node.mesh->print(node.tree_level);

  }
  if (node.material) {
  // node.material->print(node.tree_level);
  }

  //node.print_state(node.tree_level);
  std::cout << std::endl;

  for (auto &child : node.children) {
    scene_graph_print_by_node(*child);
  }
}


void Scene::state_update_recursive(Node &node)
{
  if (node.armature) {
    node.state.animated = true;
  }

  if (node.material) {
    if (node.material->diffuse && node.material->normal && node.material->specular ) {
      node.state.diffuse_specular_normal = true;
    } else if (node.material->diffuse && node.material->normal) {
      node.state.diffuse_normal = true;
    } else if (node.material->diffuse) {
      node.state.diffuse = true;
    } else {
      node.state.standard = true;
    }
  }

  for (auto &child : node.children) {
    state_update_recursive(*child);
  }
}


void Scene::upload_queue_add(Node &node) 
{
  if (node.mesh) {
    upload_queue.push_back(&node);
    render_list_add(node);
  } else if (node.armature) {
    animation_list_add(node);
  }

  for (auto &child : node.children) {
    upload_queue_add(*child);
  }
}


Node *Scene::upload_queue_pop()
{
  if (upload_queue.empty()) {
    return nullptr;
  }

  Node *node = upload_queue.back();
  if (!node) {
    std::cout << "No item left in vector" << std::endl;
  }

  upload_queue.pop_back();

  return node;
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


Node *Scene::node_find_recursive(Node &node, const std::string &name)
{
  Node *ret = nullptr;
  if (!node.name.compare(name)) {
    return &node;
  }
  for (auto &child : node.children) {
    ret = node_find_recursive(*child, name);
    if (ret)
      return ret;
  }

  return ret;
}
