#include "plugin_node_tool.h"
#include "physics_rigidbody.h"



Plugin_Node_Tool::Plugin_Node_Tool(Console &console, Scene &scene)
{
  this->console = &console;
  this->scene = &scene;
}


void Plugin_Node_Tool::cb_mouse_pressed(SDL_MouseButtonEvent *ev)
{
  auto height = scene->window_get().height_get();
  auto width = scene->window_get().width_get();
  Raycast raycast;
  POLL_DEBUG(std::cout, "casting ray x/y: " << ev->x << ", " << ev->y << " width/height: " << width << "/" << height);
  raycast.cast(*scene, ev->x, ev->y, width, height);
}


void Plugin_Node_Tool::cb_mouse_released(SDL_MouseButtonEvent *ev)
{
  POLL_DEBUG(std::cout, "released");
}


void Plugin_Node_Tool::cb_mouse_motion(SDL_MouseMotionEvent *ev)
{
}
