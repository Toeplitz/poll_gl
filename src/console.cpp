#include "console.h"
#include <glm/gtx/string_cast.hpp>


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Console::Console():
  flag_toggle(false)
{

}


Console::~Console()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Console::init(Scene &scene, GLcontext &glcontext)
{
  this->glcontext = &glcontext;
  glshader_console.load("shaders/console.v", "shaders/console.f"); 
  glcontext.uniform_locations_console_init(glshader_console);

  font.load("data/fonts/FreeMono.ttf");
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

  float x, y;
  x = 100;
  y = 100;
  text->bake_coords(text_mesh, x, y, "Martin");
  glcontext.vertex_buffers_mesh_create(text_mesh);
}


void Console::draw()
{
  if (!flag_toggle)
    return;

  glshader_console.use();
  glcontext->draw_text(*node_text);
 // glcontext->draw_mesh(*node_console->mesh_get());

}


void Console::toggle()
{
  flag_toggle = !flag_toggle;
}


