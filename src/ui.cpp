#include "ui.h"
#include <glm/gtx/string_cast.hpp>


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Ui::Ui()
{
}


Ui::~Ui()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Ui::init(Console &console, GLcontext &glcontext, Scene &scene)
{
  this->console = &console;
  this->scene = &scene;
  this->glcontext = &glcontext;

  console.command_add("light", "add", std::bind(&Ui::callback_light_create, this, _1, _2, _3));
  console.command_add("light", "list", std::bind(&Ui::callback_light_list, this, _1, _2, _3));

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


void Ui::term()
{
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


void Ui::callback_light_create(const std::string &prim, const std::string &sec, const std::string &val)
{
  Assets &assets = scene->assets_get();
  Camera *camera = scene->camera_get();
  const glm::vec3 position = camera->position_get();

  Node *sphere = &scene->load(*glcontext, "data/", "sphere.obj", MODEL_IMPORT_OPTIMIZED | MODEL_IMPORT_NO_DRAW);

  Node *node = scene->node_create("light_added");
  Light *light = node->light_create(assets, position, sphere);
  light->scale(glm::vec3(20, 20, 20));

  glcontext->vertex_buffers_light_create(light);
}


void Ui::callback_light_list(const std::string &prim, const std::string &sec, const std::string &val)
{
  Assets &assets = scene->assets_get();
  Node &root = scene->node_root_get();
  assets.light_print_all(root);
}


