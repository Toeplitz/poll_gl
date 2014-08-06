#include "console.h"
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>
#include <vector>
#include <glm/gtx/string_cast.hpp>


using namespace std::placeholders;


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Console::Console():
  flag_toggle(false)
{
  command_defaults_set();
}


Console::~Console()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Console::init(Scene &scene, GLcontext &glcontext, Window &window)
{
  this->scene = &scene;
  this->glcontext = &glcontext;

  window.keyboard_pressed_callback_set(this, &Console::keyboard_pressed_cb);
  glshader_console.load("shaders/console.v", "shaders/console.f"); 
  glcontext.uniform_locations_console_init(glshader_console);

  font.load("data/fonts/Vera.ttf");
  font.bake();
  Texture &texture = font.texture_get();
  glcontext.texture_single_channel_create(texture);

  node_console = scene.node_create("console");
  Mesh *mesh = node_console->mesh_create(scene.assets_get());
  mesh->quad_generate(1.f);
  glcontext.vertex_buffers_mesh_create(mesh);

  node_text = scene.node_create("entry_box", node_console);
  Text *text = node_text->text_create(&font, scene.assets_get());
  Mesh *text_mesh = node_text->mesh_get();

  text->string_set("Fragmic");
  text->bake_coords(node_text->mesh_get(), 10, 10);
  glcontext.vertex_buffers_mesh_create(text_mesh, 1048 * sizeof(glm::vec3));
}


void Console::draw()
{
  if (!flag_toggle)
    return;

  glshader_console.use();
  glcontext->draw_text(*node_text);
 // glcontext->draw_mesh(*node_console->mesh_get());

}


void Console::keyboard_pressed_cb(SDL_Keysym *keysym)
{
  Text *text;
  Mesh *mesh;

  if (!flag_toggle)
    return;

  text = node_text->text_get();
  mesh = node_text->mesh_get();

  switch (keysym->sym) {
    case SDLK_RETURN:
      break;
    case SDLK_SPACE:
      text->string_append(" ");
      text->bake_coords(mesh, 10, 10);
      glcontext->vertex_buffers_mesh_update(mesh);
      break;
    case SDLK_BACKSPACE:
      if (text->string_len() > 2) {
        text->string_erase_last();
        text->bake_coords(mesh, 10, 10);
        glcontext->vertex_buffers_mesh_update(mesh);
      }
      break;
    default:
      if((keysym->sym >= SDLK_a && keysym->sym <= SDLK_z) ||
          (keysym->sym >= SDLK_0 && keysym->sym <= SDLK_9) ||
          keysym->sym == SDLK_PERIOD) {
        std::string key_input(SDL_GetKeyName(keysym->sym));
        std::transform(key_input.begin(), key_input.end(), key_input.begin(), ::tolower);
        text->string_append(key_input);
        text->bake_coords(mesh, 10, 10);
        glcontext->vertex_buffers_mesh_update(mesh);
      } 
      break;
  }

}


void Console::toggle()
{
  Text *text;
  Mesh *mesh;

  text = node_text->text_get();
  mesh = node_text->mesh_get();

  if (flag_toggle) {
    std::string &cmd_full = text->string_get();
    std::string cmd = cmd_full.substr(2, cmd_full.size());
    command_parse(cmd);


  } else {
    text->string_set("> ");
    text->bake_coords(mesh, 10, 10);
    glcontext->vertex_buffers_mesh_update(mesh);
  }

  flag_toggle = !flag_toggle;

}


bool Console::active()
{
  return flag_toggle;
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


void Console::callback_camera_fov_set(const float val)
{
  Camera *camera = scene->camera_get();
  camera->fov_set(val);
}


void Console::callback_light_create(const float val)
{
  Assets &assets = scene->assets_get();
  Camera *camera = scene->camera_get();
  const glm::vec3 position = camera->position_get();

  Node *sphere = &scene->model_load("data/", "sphere.obj", MODEL_IMPORT_OPTIMIZED | MODEL_IMPORT_NO_DRAW);

  Node *node = scene->node_create("light_added");
  Light *light = node->light_create(assets, position, sphere);
  light->scale(glm::vec3(20, 20, 20));

  glcontext->vertex_buffers_light_create(light);
  std::cout << "Adding light at position: " << glm::to_string(position) << std::endl;
}


void Console::command_defaults_set()
{
  commands["fov"] = std::bind(&Console::callback_camera_fov_set, this, _1);
  commands["light"] = std::bind(&Console::callback_light_create, this, _1);
}



void Console::command_exec(const std::string &key, const std::string &value)
{
  float num = ::atof(value.c_str());

  if (commands.find(key) != commands.end()) {
    commands.at(key)(num);
  } else {
    std::cout << "Unknown command: '" << key << "'" << std::endl;
  }

}

void Console::command_parse(std::string &cmd_full)
{
  std::istringstream iss(cmd_full);
  std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                  std::istream_iterator<std::string>{}};

  if (tokens.size() > 1) {
    command_exec(tokens[0], tokens[1]);
  }

}
