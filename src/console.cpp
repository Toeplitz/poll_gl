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


void Console::init(Scene &scene, GLcontext &glcontext, Window &window)
{
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

  text->string_set("Martin lol");
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

  text->string_append("o");
  text->bake_coords(mesh, 10, 10);
  glcontext->vertex_buffers_mesh_update(mesh);

  std::cout << "Vertices: " << mesh->num_vertices_get() << std::endl;
  
  switch (keysym->sym) {

    default:
      break;
  }

}


void Console::toggle()
{
  flag_toggle = !flag_toggle;
}


