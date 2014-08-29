#include "ui.h"
#include <glm/gtx/string_cast.hpp>


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Ui::init(Console &console, GLcontext &glcontext, Scene &scene)
{
  this->console = &console;
  this->scene = &scene;
  this->glcontext = &glcontext;

  console.command_add("asset", "list", std::bind(&Ui::callback_asset_list, this, _1, _2, _3));
  console.command_add("light", "add", std::bind(&Ui::callback_light_create, this, _1, _2, _3));
  console.command_add("light", "disable", std::bind(&Ui::callback_light_disable, this, _1, _2, _3));
  console.command_add("light", "enable", std::bind(&Ui::callback_light_enable, this, _1, _2, _3));
  console.command_add("light", "list", std::bind(&Ui::callback_light_list, this, _1, _2, _3));
  console.command_add("light", "print", std::bind(&Ui::callback_light_print, this, _1, _2, _3));
  console.command_add("light", "select", std::bind(&Ui::callback_light_select, this, _1, _2, _3));
  console.command_add("light", "type", std::bind(&Ui::callback_light_select, this, _1, _2, _3));
  console.command_add("node", "manipulator", std::bind(&Ui::callback_node_manipulator, this, _1, _2, _3));
  console.command_add("scene", "list", std::bind(&Ui::callback_scene_list, this, _1, _2, _3));

  Font *font = console.font_get();

  { 
    node_label_cam = scene.node_create("ui_label_cam");
    Text *text = node_label_cam->text_create(font, scene.assets_get());
    const std::string cam_pos_str("x, y, z");
    text->string_set(cam_pos_str); 
    text->bake(nullptr, node_label_cam->mesh_get(), CONSOLE_X, 700);
    glcontext.vertex_buffers_mesh_create(node_label_cam->mesh_get(), 1048 * sizeof(glm::vec3));
  }

  {
    node_label_fps = scene.node_create("ui_label_fps");
    Text *text = node_label_fps->text_create(font, scene.assets_get());
    const std::string fps_str("fps");
    text->string_set(fps_str); 
    text->bake(nullptr, node_label_fps->mesh_get(), CONSOLE_X, 680);
    glcontext.vertex_buffers_mesh_create(node_label_fps->mesh_get(), 1048 * sizeof(glm::vec3));
  }

}


void Ui::draw(const std::string &fps_text)
{
  {
    Camera *camera = scene->camera_get();
    Text *text = node_label_cam->text_get();
    auto pos = camera->position_get();
    const std::string cam_pos_str(glm::to_string(pos));
    text->string_set(cam_pos_str); 
    text->bake(glcontext, node_label_cam->mesh_get(), CONSOLE_X, 700);
  }
  
  {
    Text *text = node_label_fps->text_get();
    text->string_set(fps_text); 
    text->bake(glcontext, node_label_fps->mesh_get(), CONSOLE_X, 680);
  }

  glcontext->draw_text(*node_label_cam);
  glcontext->draw_text(*node_label_fps);
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


void Ui::callback_asset_list(const std::string &prim, const std::string &sec, const std::string &val)
{
  Assets &assets = scene->assets_get();
  assets.print_all(scene->node_root_get());
}


void Ui::callback_light_create(const std::string &prim, const std::string &sec, const std::string &val)
{
  Assets &assets = scene->assets_get();
  Camera *camera = scene->camera_get();
  const glm::vec3 position = camera->position_get();

  Node *node = scene->node_create("light_added");
  node->translate(position);
  node->scale(glm::vec3(30, 30, 30));
  Light *light = node->light_create(assets, Light::POINT);
  light_active_set(light);
  callback_light_list(prim, sec, val);
}


void Ui::callback_light_disable(const std::string &prim, const std::string &sec, const std::string &val)
{
  Assets &assets = scene->assets_get();

  if (!light_active)
    return;

  assets.light_deactivate(light_active);
  callback_light_list(prim, sec, val);
}


void Ui::callback_light_enable(const std::string &prim, const std::string &sec, const std::string &val)
{
  Assets &assets = scene->assets_get();

  if (!light_active)
    return;

  assets.light_activate(light_active);
  callback_light_list(prim, sec, val);
}


void Ui::callback_light_type(const std::string &prim, const std::string &sec, const std::string &val)
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

void Ui::callback_light_print(const std::string &prim, const std::string &sec, const std::string &val)
{
  if (!light_active)
    return;

  light_active->print(0);
}


void Ui::callback_light_select(const std::string &prim, const std::string &sec, const std::string &val)
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


void Ui::callback_light_list(const std::string &prim, const std::string &sec, const std::string &val)
{
  Assets &assets = scene->assets_get();
  Node &root = scene->node_root_get();
  assets.light_print_all(root);
  std::cout << "\tCurrent selected light: " << light_active << std::endl;
}


void Ui::callback_node_manipulator(const std::string &prim, const std::string &sec, const std::string &val)
{
  int toggle = 0;

  if (val.size() > 0)
    toggle = ::atoi(val.c_str());

  //scene->manipulator_toggle
 
}


void Ui::callback_scene_list(const std::string &prim, const std::string &sec, const std::string &val)
{
  int compact = 0;

  if (val.size() > 0)
    compact = ::atoi(val.c_str());

  scene->scene_graph_print(compact);
 
}


void Ui::light_active_set(Light *light)
{
  light_active = light;

}
