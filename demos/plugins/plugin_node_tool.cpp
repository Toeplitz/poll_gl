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
  Physics &physics = scene->physics_get();
  auto height = scene->window_get().height_get();
  auto width = scene->window_get().width_get();
  auto hp = raycast.cast(*scene, ev->x, ev->y, width, height);

  if (!hp)
    return;

  Physics_Rigidbody *rb = hp->node_ptr->physics_rigidbody_get();
  rb->constraint_create(*hp);
  physics.rigidbody_constraint_add(rb);

  hp->print();
  this->hitpoint_last = hp;
  this->mouse_down = true;
}


void Plugin_Node_Tool::cb_mouse_released(SDL_MouseButtonEvent *ev)
{
  Physics &physics = scene->physics_get();
  this->mouse_down = false;
  if (!hitpoint_last)
    return;

  Node *node = hitpoint_last->node_ptr;
  if (!node) {
    POLL_ERROR(std::cerr, "No node attached to hitpoint");
    return;
  }
  Physics_Rigidbody *rb = node->physics_rigidbody_get();
  if (!rb) {
    POLL_ERROR(std::cerr, "No rigidbody on object which was clicked??");
    return;
  }

  //physics.rigidbody_constraint_delete(rb);
  //rb->constraint_delete();
  hitpoint_last = nullptr;
}


void Plugin_Node_Tool::cb_mouse_motion(SDL_MouseMotionEvent *ev)
{

  if (!mouse_down)
    return;

  if (!hitpoint_last)
    return;

  Node *node = hitpoint_last->node_ptr;
  if (!node) {
    POLL_ERROR(std::cerr, "No node attached to hitpoint");
    return;
  }
  Physics_Rigidbody *rb = node->physics_rigidbody_get();

  if (!rb) {
    POLL_ERROR(std::cerr, "No rigidbody on object which was clicked??");
    return;
  }

  auto dof6 = rb->bt_dof6_get();

  if (!dof6) {
    POLL_ERROR(std::cerr, "No dof5 object created??");
    return;
  }

  auto height = scene->window_get().height_get();
  auto width = scene->window_get().width_get();
  auto hp = raycast.cast(*scene, ev->x, ev->y, width, height);
  btVector3 newRayTo = btVector3(hp->world_ray.x, hp->world_ray.y, hp->world_ray.z);
  btVector3 rayFrom;
  btVector3 oldPivotInB = rb->bt_dof6_get()->getFrameOffsetA().getOrigin();

}



