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
  console.command_add("object", "add", std::bind(&Ui::callback_object_add, this, _1, _2, _3));
  console.command_add("node", "manipulator", std::bind(&Ui::callback_node_manipulator, this, _1, _2, _3));
  console.command_add("scene", "list", std::bind(&Ui::callback_scene_list, this, _1, _2, _3));

  Font *font = console.font_get();

  { 
    node_label_cam = scene.node_create("ui_label_cam");
    Text *text = node_label_cam->text_create(font, scene);
    const std::string cam_pos_str("x, y, z");
    text->string_set(cam_pos_str); 
    text->bake(nullptr, node_label_cam->mesh_get(), CONSOLE_X, 700);
    glcontext.vertex_buffers_mesh_create(node_label_cam->mesh_get(), 1048 * sizeof(glm::vec3));
  }

  {
    node_label_fps = scene.node_create("ui_label_fps");
    Text *text = node_label_fps->text_create(font, scene);
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



void Ui::callback_object_add(const std::string &prim, const std::string &sec, const std::string &val)
{
  Camera *camera = scene->camera_get();
  auto pos = camera->position_get();
  int num = 1;

  if (val.size() > 0)
    num = ::atoi(val.c_str());

  for (int i = 0; i < num; i++) {
    Node &node = scene->load("data/", "sphere.obj", MODEL_IMPORT_OPTIMIZED);
    node.translate(*scene, pos);
    node.physics_rigidbody_create(*scene, false, Physics_Rigidbody::SPHERE, Physics_Rigidbody::DYNAMIC, 10.f);
  }

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


