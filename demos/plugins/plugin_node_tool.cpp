#include "plugin_node_tool.h"
#include "physics_rigidbody.h"
#include <glm/gtx/string_cast.hpp>

/*


Usage:


't': translate mode:
's': scale mode
'r': rotate mode

'x': manipulate x-axis sub-mode.
'y': manipulate y-axis sub-mode.
'z': manipulate z-axis sub-mode.


*/


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
  auto world_ray_anton = raycast.cast_empty(*scene, ev->x, ev->y, width, height);
  auto world_ray = raycast.get_ray_to(*scene, ev->x, ev->y, width, height);

  POLL_DEBUG(std::cout, "cast_empty: " << glm::to_string(world_ray_anton));
  POLL_DEBUG(std::cout, "get_ray_to: " << glm::to_string(world_ray));

  btVector3 newRayTo = btVector3(world_ray.x, world_ray.y, world_ray.z);
  //btVector3 newRayTo = btVector3(world_ray_tutor.x, world_ray_tutor.y, world_ray_tutor.z);
  //btVector3 newRayTo = btVector3(world_ray_anton.x, world_ray_anton.y, world_ray_anton.z);
  btVector3 rayFrom;

  vec3 camera_pos = scene->camera_get()->position_get();
  float dist = glm::length(hitpoint_last->world_hitpoint - hitpoint_last->ray_from);

  rayFrom = btVector3(camera_pos.x, camera_pos.y, camera_pos.z);
  btVector3 dir = newRayTo - rayFrom;
  dir.normalize();
  dir *= dist;

  vec3 dir_glm = vec3(dir.getX(), dir.getY(), dir.getZ());
  POLL_DEBUG(std::cout, "dir: " << glm::to_string(dir_glm));
  btVector3 newPivotB = rayFrom + dir;
  vec3 newPivotB_glm = vec3(newPivotB.getX(), newPivotB.getY(), newPivotB.getZ());
  POLL_DEBUG(std::cout, "newPivotB: " << glm::to_string(newPivotB_glm));

  if (rb->type_get() == Physics_Rigidbody::KINEMATIC) {
    //node->translate(*scene, glm::vec3(newPivotB.getX(), 0, 0));
    //
    Node *ptr = node;
    if (node->grab_parent) {
      ptr = node->parent_get();
    }

    mat4 t = ptr->transform_global_translate_get();
    vec3 last_pos = vec3(t[3][0], 0, 0);

    vec3 diff = vec3(newPivotB.getX(), 0, 0) - last_pos;
    ptr->translate(*scene, diff);
    //vec3 v = glm::vec3(newPivotB.getX(), 0, 0);
  } else {
    dof6->getFrameOffsetA().setOrigin(newPivotB);
  }

  //POLL_DEBUG(std::cout, "dist: " << dist);

  printf("newPivotB=%f,%f,%f\n",newPivotB.getX(),newPivotB.getY(),newPivotB.getZ());

}



