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


void Scene::animation_list_update_transforms(Node &node, const double dt)
{
  glm::mat4 transform = node.transform_local_current;
  Node *parent = node.parent;

  if (node.keyframe_total_num_get()) {
    double factor = node.step_time(dt);
    glm::mat4 m = node.keyframe_interpolate(factor);
    node.local_transform_current_set(m);
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


Node &Scene::model_load(const std::string &prefix, const std::string &filename, bool draw) 
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
    node.material->print(node.tree_level);
  }

  node.print_state(node.tree_level);
  std::cout << std::endl;

  for (auto &child : node.children) {
    scene_graph_print_by_node(*child);
  }
}


Node *Scene::node_create(const std::string &name)
{
  std::unique_ptr<Node> node(new Node(name));
  std::unique_ptr<Mesh> mesh(new Mesh());
  std::unique_ptr<Material> material(new Material());

  Node *node_ptr  = node.get();
  node->mesh = mesh.get();
  node->material = material.get();

  root.child_add(std::move(node), root.tree_level + 1);
  assets.mesh_add(std::move(mesh));
  assets.material_add(std::move(material));

  return node_ptr;
}


Node &Scene::node_root_get()
{
  return root;
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
    } else if (node.material->cubemap) {
      std::cout << "Updating material state" << std::endl;
      if (node.material->cubemap->type == CUBEMAP_SKYBOX) {
        node.state.cubemap_skybox = true;
        std::cout << "Found skybox" << std::endl;
      } else if (node.material->cubemap->type == CUBEMAP_REFLECTION) {
        std::cout << "Found reflection" << std::endl;
        node.state.cubemap_reflect = true;
      } else {
        std::cout << "Error: cubemap type not recognized" << std::endl;
      }
      node.state.standard = false;
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

  state_update_recursive(node);

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

  if (!ret) {
    std::cout << "Error: could not find node '" << name << "'" << std::endl;
  }

  return ret;
}
