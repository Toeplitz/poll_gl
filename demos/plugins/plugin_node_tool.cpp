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

  physics.rigidbody_constraint_delete(rb);
  rb->constraint_delete();
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
  //auto world_ray = raycast.cast_empty(*scene, ev->x, ev->y, width, height);
  auto world_ray = raycast.get_ray_to(*scene, ev->x, ev->y, width, height);

  //POLL_DEBUG(std::cout, "cast_empty: " << glm::to_string(world_ray));
  //POLL_DEBUG(std::cout, "get_ray_to: " << glm::to_string(world_ray2));

  btVector3 newRayTo = btVector3(world_ray.x, world_ray.y, world_ray.z);
  btVector3 rayFrom;

  vec3 camera_pos = scene->camera_get()->position_get();
  float dist = glm::length(hitpoint_last->world_hitpoint - hitpoint_last->ray_from);

  rayFrom = btVector3(camera_pos.x, camera_pos.y, camera_pos.z);
  btVector3 dir = newRayTo - rayFrom;
  dir.normalize();
  dir *= dist;
  btVector3 newPivotB = rayFrom + dir;

  if (rb->type_get() == Physics_Rigidbody::KINEMATIC) {
    //node->translate(*scene, glm::vec3(newPivotB.getX(), 0, 0));

    mat4 t = node->transform_translate_get();
    vec3 last_pos = vec3(t[3][0], 0, 0);

    vec3 diff = vec3(newPivotB.getX(), 0, 0) - last_pos;
    POLL_DEBUG(std::cout, glm::to_string(last_pos));
    POLL_DEBUG(std::cout, glm::to_string(diff));
    node->translate(*scene, diff);
    //vec3 v = glm::vec3(newPivotB.getX(), 0, 0);
  } else {
    dof6->getFrameOffsetA().setOrigin(newPivotB);
  }

  //POLL_DEBUG(std::cout, "dist: " << dist);

  printf("newPivotB=%f,%f,%f\n",newPivotB.getX(),newPivotB.getY(),newPivotB.getZ());

}



