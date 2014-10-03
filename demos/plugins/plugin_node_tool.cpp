#include "plugin_node_tool.h"
#include "physics_rigidbody.h"
#include <glm/gtx/string_cast.hpp>



Plugin_Node_Tool::Plugin_Node_Tool(Console &console, Scene &scene)
{
  this->console = &console;
  this->scene = &scene;
}


void Plugin_Node_Tool::cb_mouse_pressed(SDL_MouseButtonEvent *ev)
{
  auto height = scene->window_get().height_get();
  auto width = scene->window_get().width_get();
//  POLL_DEBUG(std::cout, "casting ray x/y: " << ev->x << ", " << ev->y << " width/height: " << width << "/" << height);
  Raycast_Hitpoint *hp = raycast.cast(*scene, ev->x, ev->y, width, height);

  if (!hp)
    return;

  POLL_DEBUG(std::cout, "clicked: " << hp->node_ptr->name_get() << " pos: " << glm::to_string(hp->world_hitpoint));
  this->hitpoint_last = hp;

}


void Plugin_Node_Tool::cb_mouse_released(SDL_MouseButtonEvent *ev)
{
}


void Plugin_Node_Tool::cb_mouse_motion(SDL_MouseMotionEvent *ev)
{
  if (!hitpoint_last)
    return;

  auto height = scene->window_get().height_get();
  auto width = scene->window_get().width_get();
  vec3 v = raycast.cast_empty(*scene, ev->x, ev->y, width, height);
  vec3 difference = v - hitpoint_last->world_hitpoint;

  POLL_DEBUG(std::cout, "released, difference: " << glm::to_string(difference));
  hitpoint_last->node_ptr->translate(*scene, vec3(difference.x, 0, 0));
  hitpoint_last->world_hitpoint = v;
}



