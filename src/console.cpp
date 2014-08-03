#include "console.h"


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

  text.font_read("data/fonts/FreeMono.ttf");
  text.font_bitmap_bake();
  Texture &texture = text.font_texture_get();

  glshader_console.load("shaders/console.v", "shaders/console.f"); 
  glshader_console.use();

  node = scene.node_create("console");
  Mesh *mesh = node->mesh_create(scene.assets_get());
  mesh->quad_generate(1.f);

  glcontext.vertex_buffers_mesh_create(mesh);
  glcontext.uniform_textures_font_init(glshader_console);
  glcontext.texture_font_bitmap_create(texture);

}


void Console::draw()
{
  if (!flag_toggle)
    return;

  glshader_console.use();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glcontext->draw_text(text);
  glcontext->draw_node(*node);
  glDisable(GL_BLEND);

}


void Console::toggle()
{
  flag_toggle = !flag_toggle;
}


