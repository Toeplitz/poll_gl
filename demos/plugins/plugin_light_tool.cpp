#include "plugin_light_tool.h"


Plugin_Light_Tool::Plugin_Light_Tool(Console &console, Scene &scene)
{
  this->console = &console;
  this->scene = &scene;

  console.command_add("light", "add", std::bind(&Plugin_Light_Tool::callback_light_create, this, _1, _2, _3));
  console.command_add("light", "disable", std::bind(&Plugin_Light_Tool::callback_light_disable, this, _1, _2, _3));
  console.command_add("light", "enable", std::bind(&Plugin_Light_Tool::callback_light_enable, this, _1, _2, _3));
  console.command_add("light", "list", std::bind(&Plugin_Light_Tool::callback_light_list, this, _1, _2, _3));
  console.command_add("light", "print", std::bind(&Plugin_Light_Tool::callback_light_print, this, _1, _2, _3));
  console.command_add("light", "select", std::bind(&Plugin_Light_Tool::callback_light_select, this, _1, _2, _3));
  console.command_add("light", "type", std::bind(&Plugin_Light_Tool::callback_light_type, this, _1, _2, _3));
}


void Plugin_Light_Tool::callback_light_create(const std::string &prim, const std::string &sec, const std::string &val)
{
  Camera *camera = scene->camera_get();
  const glm::vec3 position = camera->position_get();

  Node *node = scene->node_create("light_added");
  node->translate(*scene, position);
  node->scale(*scene, glm::vec3(30, 30, 30));
  Light *light = node->light_create(*scene, Light::POINT);
  light->properties_color_set(glm::vec3(1, 1, 1));
  light_active_set(light);
  callback_light_list(prim, sec, val);
}


void Plugin_Light_Tool::callback_light_disable(const std::string &prim, const std::string &sec, const std::string &val)
{
  if (!light_active)
    return;

  light_active->node_ptr_get()->active_set(*scene, false);
}


void Plugin_Light_Tool::callback_light_enable(const std::string &prim, const std::string &sec, const std::string &val)
{
  if (!light_active)
    return;

  light_active->node_ptr_get()->active_set(*scene, true);
}


void Plugin_Light_Tool::callback_light_type(const std::string &prim, const std::string &sec, const std::string &val)
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

void Plugin_Light_Tool::callback_light_print(const std::string &prim, const std::string &sec, const std::string &val)
{
  if (!light_active)
    return;

  light_active->print(0);
}


void Plugin_Light_Tool::callback_light_select(const std::string &prim, const std::string &sec, const std::string &val)
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
  callback_light_list(prim, sec, val);
}


void Plugin_Light_Tool::callback_light_list(const std::string &prim, const std::string &sec, const std::string &val)
{
  Assets &assets = scene->assets_get();
  Node &root = scene->node_root_get();
  assets.light_print_all(root);
  std::cout << "\tCurrent selected light: " << light_active << std::endl;
}


void Plugin_Light_Tool::custom_draw_callback()
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
  if (light_active) {
    light_active->node_ptr_get()->active_set(*scene, false);
  }

  light_active = light;
  light_active->node_ptr_get()->active_set(*scene, true);
}
