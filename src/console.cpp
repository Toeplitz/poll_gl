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

  font.load("data/fonts/FreeMono.ttf");
  font.bake();
  Texture &texture = font.texture_get();

  node = scene.node_create("console");
  Mesh *mesh = node->mesh_create(scene.assets_get());
  mesh->quad_generate(1.f);

  Node *text_node = scene.node_create("entry_box", node);
  Text *text = text_node->text_create(&font, scene.assets_get());

  glshader_console.load("shaders/console.v", "shaders/console.f"); 

  glcontext.vertex_buffers_mesh_create(mesh);
  glcontext.texture_single_channel_create(texture);
  glcontext.uniform_locations_console_init(glshader_console);

  float x, y;
  x = 0;
  y = 10;
  const std::vector<glm::vec4> coords = text->bake_coords(x, y, "Martin");

  for (size_t i = 0; i < coords.size(); i++) {
    std::cout << glm::to_string(coords[i]) << std::endl;
  }

}


void Console::draw()
{
  if (!flag_toggle)
    return;

  glshader_console.use();
  GL_ASSERT(glEnable(GL_BLEND));
  GL_ASSERT(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
 // glcontext->draw_text(text);
  glcontext->draw_mesh(*node->mesh_get());
  GL_ASSERT(glDisable(GL_BLEND));

}


void Console::toggle()
{
  flag_toggle = !flag_toggle;
}


