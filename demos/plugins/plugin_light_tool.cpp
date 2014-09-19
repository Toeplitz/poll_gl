#include "plugin_light_tool.h"
#include <glm/gtx/string_cast.hpp>


Plugin_Light_Tool::Plugin_Light_Tool(Console &console, Scene &scene)
{
  this->console = &console;
  this->scene = &scene;

  console.command_add("light", "add", std::bind(&Plugin_Light_Tool::console_cmd_light_create, this, _1, _2, _3));
  console.command_add("light", "disable", std::bind(&Plugin_Light_Tool::console_cmd_light_disable, this, _1, _2, _3));
  console.command_add("light", "enable", std::bind(&Plugin_Light_Tool::console_cmd_light_enable, this, _1, _2, _3));
  console.command_add("light", "list", std::bind(&Plugin_Light_Tool::console_cmd_light_list, this, _1, _2, _3));
  console.command_add("light", "print", std::bind(&Plugin_Light_Tool::console_cmd_light_print, this, _1, _2, _3));
  console.command_add("light", "select", std::bind(&Plugin_Light_Tool::console_cmd_light_select, this, _1, _2, _3));
  console.command_add("light", "type", std::bind(&Plugin_Light_Tool::console_cmd_light_type, this, _1, _2, _3));
}


void Plugin_Light_Tool::console_cmd_light_create(const std::string &prim, const std::string &sec, const std::string &val)
{
  Camera *camera = scene->camera_get();
  const glm::vec3 position = camera->position_get();

  Node *node = scene->node_create("light_added");
  node->translate(*scene, position);
  node->scale(*scene, glm::vec3(30, 30, 30));
  Light *light = node->light_create(*scene, Light::POINT);
  light->properties_color_set(glm::vec3(1, 1, 1));
  light_active_set(light);
  console_cmd_light_list(prim, sec, val);
}


void Plugin_Light_Tool::console_cmd_light_disable(const std::string &prim, const std::string &sec, const std::string &val)
{
  if (!light_active)
    return;

  light_active->node_ptr_get()->active_set(*scene, false);
}


void Plugin_Light_Tool::console_cmd_light_enable(const std::string &prim, const std::string &sec, const std::string &val)
{
  if (!light_active)
    return;

  light_active->node_ptr_get()->active_set(*scene, true);
}


void Plugin_Light_Tool::console_cmd_light_type(const std::string &prim, const std::string &sec, const std::string &val)
{
  if (!light_active)
    return;

  int type = ::atoi(val.c_str());
  switch (type) {
    case 0:
      light_active->properties_type_set(Light::DIRECTIONAL);
      break;
    case 1:
      light_active->properties_type_set(Light::POINT);
      break;
    case 2:
      light_active->properties_type_set(Light::SPOT);
      break;
    default:
      std::cout << "Light type: '" << type << "'is unknown. Valid choices are: 0 (directional), 1 (point), 2 (spot)" << std::endl;
      break;
  }

}

void Plugin_Light_Tool::console_cmd_light_print(const std::string &prim, const std::string &sec, const std::string &val)
{
  if (!light_active)
    return;

  light_active->print(0);
}


void Plugin_Light_Tool::console_cmd_light_select(const std::string &prim, const std::string &sec, const std::string &val)
{
  if (val.size() <= 0)
    return;

  int light_num = ::atoi(val.c_str());
  Light *light = scene->assets_get().light_ith_get(light_num);

  if (!light) {
    std::cout << "No light with index: " << light_num << std::endl;
    light_active_set(nullptr);
    return;
  }

  light_active_set(light);
  console_cmd_light_list(prim, sec, val);
}


void Plugin_Light_Tool::console_cmd_light_list(const std::string &prim, const std::string &sec, const std::string &val)
{
  Assets &assets = scene->assets_get();
  Node &root = scene->node_root_get();
  assets.light_print_all(root);
  std::cout << "\tCurrent selected light: " << light_active << std::endl;
}


void Plugin_Light_Tool::custom_callback_draw()
{
  Assets &assets = scene->assets_get();
  Stock_Shaders &shader = assets.stock_shaders_get();
  GLcontext &glcontext = scene->glcontext_get();

  shader.world_basic_color.use();

  auto &lights = assets.light_active_get();
  for (auto &light: lights) {
    Node *node = light->node_ptr_get();

    for (auto &child : node->children_get()) {
      mat4 &transform = child->transform_global_get();
      glcontext.uniform_buffers_update_matrices(transform);
      glcontext.draw_mesh(*child);
    }
  }
}


void Plugin_Light_Tool::light_active_set(Light *light)
{
  if (light == light_active)
    return;

  if (light_active) {
    Gimbal_Nodes &gimbal_nodes = light_active->gimbal_nodes_get();
    gimbal_nodes.x->active_set(*scene, false);
    gimbal_nodes.y->active_set(*scene, false);
    gimbal_nodes.z->active_set(*scene, false);
  }

  light_active = light;

  if (light_active) {
    Gimbal_Nodes &gimbal_nodes = light_active->gimbal_nodes_get();
    gimbal_nodes.x->active_set(*scene, true);
    gimbal_nodes.y->active_set(*scene, true);
    gimbal_nodes.z->active_set(*scene, true);
  }
}


void Plugin_Light_Tool::light_callback_create(Node *node_ptr)
{
  Light *light = node_ptr->light_get();

  if (!light) {
    POLL_ERROR(std::cerr, "Light callback node has no light?");
    return;
  }

  {
    unsigned int lamp_type = light->properties_type_get();
    if (lamp_type == Light::POINT || lamp_type == Light::SPOT) {
      Assets &assets = scene->assets_get();
      Stock_Nodes &stock_nodes = assets.stock_nodes_get();
      Gimbal_Nodes &gimbal_nodes = light->gimbal_nodes_get();

      {
        Node *node_symbol_cone = stock_nodes.sphere_get();
        Node *node = scene->node_create("light_symbol", node_ptr, TRANSFORM_INHERIT_POSITION_ONLY);
        node->scale(*scene, glm::vec3(0.1, 0.1, 0.1));
        node->mesh_set(node_symbol_cone->mesh_get());
        node->physics_rigidbody_create(*scene, false, Physics_Rigidbody::BOX, Physics_Rigidbody::KINEMATIC, 0);
        gimbal_nodes.center = node;
      }

      {


      }

      Node *node_symbol_disk = stock_nodes.disk_get();
      {
        Node *node = scene->node_create("light_disk", node_ptr);
        node->mesh_set(node_symbol_disk->mesh_get());
        node->physics_rigidbody_create(*scene, false, Physics_Rigidbody::TRIANGLE_MESH, Physics_Rigidbody::KINEMATIC, 0);
        node->active_set(*scene, false);
        gimbal_nodes.x = node;
      }
      {
        Node *node = scene->node_create("light_disk", node_ptr);
        node->mesh_set(node_symbol_disk->mesh_get());
        node->physics_rigidbody_create(*scene, false, Physics_Rigidbody::TRIANGLE_MESH, Physics_Rigidbody::KINEMATIC, 0);
        node->active_set(*scene, false);
        node->rotate(*scene, M_PI / 2, glm::vec3(1, 0, 0));
        gimbal_nodes.y = node;
      }
      {
        Node *node = scene->node_create("light_disk", node_ptr);
        node->mesh_set(node_symbol_disk->mesh_get());
        node->physics_rigidbody_create(*scene, false, Physics_Rigidbody::TRIANGLE_MESH, Physics_Rigidbody::KINEMATIC, 0);
        node->active_set(*scene, false);
        node->rotate(*scene, M_PI / 2, glm::vec3(0, 0, 1));
        gimbal_nodes.z = node;
      }

      gimbal_nodes.center->raycast_collide_callback_set(std::bind(&Plugin_Light_Tool::raycast_collide_callback_gimbal_center, this, _1, _2));
      gimbal_nodes.x->raycast_collide_callback_set(std::bind(&Plugin_Light_Tool::raycast_collide_callback_gimbal_x, this, _1, _2));
      gimbal_nodes.y->raycast_collide_callback_set(std::bind(&Plugin_Light_Tool::raycast_collide_callback_gimbal_y, this, _1, _2));
      gimbal_nodes.z->raycast_collide_callback_set(std::bind(&Plugin_Light_Tool::raycast_collide_callback_gimbal_z, this, _1, _2));
    }
  }

}


void Plugin_Light_Tool::raycast_collide_callback_gimbal_center(Node &node, vec3 &position)
{
  Light *light = node.parent_get()->light_get();

  if (!light) {
    POLL_ERROR(std::cerr, "Light callback node has no light?");
    return;
  }

  POLL_DEBUG(std::cout, "Clicked on gimbal center of light: " << node.name_get() << " at position: " << glm::to_string(position));
  light_active_set(light);
}


void Plugin_Light_Tool::raycast_collide_callback_gimbal_x(Node &node, vec3 &position)
{
  POLL_DEBUG(std::cout, "Clicked gimbal x");
}


void Plugin_Light_Tool::raycast_collide_callback_gimbal_y(Node &node, vec3 &position)
{
  POLL_DEBUG(std::cout, "Clicked gimbal y");
}


void Plugin_Light_Tool::raycast_collide_callback_gimbal_z(Node &node, vec3 &position)
{
  POLL_DEBUG(std::cout, "Clicked gimbal z");
}

