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

  keypress_map[SDLK_x] = std::make_pair(false, "maniplation on x-axis");
  keypress_map[SDLK_y] = std::make_pair(false, "maniplation on y-axis");
  keypress_map[SDLK_z] = std::make_pair(false, "maniplation on z-axis");
  keypress_map[SDLK_t] = std::make_pair(false, "translate mode");
  keypress_map[SDLK_s] = std::make_pair(false, "scale mode");
  keypress_map[SDLK_LSHIFT] = std::make_pair(false, "shift");
}


void Plugin_Node_Tool::cb_keyboard_pressed(SDL_Keysym *keysym)
{
  if (keypress_map.count(keysym->sym) == 0)
    return;

  keypress_map[keysym->sym].first = !keypress_map[keysym->sym].first;

  if (keypress_map[keysym->sym].first) {
    std::cout << keypress_map[keysym->sym].second << ": " << "enabled" << std::endl;
  } else {
    std::cout << keypress_map[keysym->sym].second << ": " << "disabled" << std::endl;
  }
}


void Plugin_Node_Tool::cb_keyboard_released(SDL_Keysym *keysym)
{
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


    if (keypress_map[SDLK_t].first) {

      vec3 new_pos(0, 0, 0);
      mat4 t = ptr->transform_global_translate_get();
      vec3 last_pos = vec3(t[3][0], t[3][1], t[3][2]);
      vec3 diff = vec3(newPivotB.getX(), newPivotB.getY(), newPivotB.getZ()) - last_pos;

      if (keypress_map[SDLK_x].first) {
        new_pos.x = diff.x;
      } 
      if (keypress_map[SDLK_y].first) {
        new_pos.y = diff.y;
      }
      if (keypress_map[SDLK_z].first) {
        new_pos.z = diff.z;
      }

      ptr->translate(*scene, new_pos);
    }
    
    if (keypress_map[SDLK_s].first) {
      vec3 new_pos(1, 1, 1);
      mat4 s = ptr->transform_global_scale_get();
      vec3 last_scale = vec3(s[0][0], s[1][1], s[2][2]);
      vec3 diff = glm::abs(vec3(newPivotB.getX(), newPivotB.getY(), newPivotB.getZ()) + last_scale);
      POLL_DEBUG(std::cout, "last_scale: " << glm::to_string(last_scale));
      POLL_DEBUG(std::cout, "diff: " << glm::to_string(diff));

      if (keypress_map[SDLK_x].first) {
        new_pos.x = diff.x;
      } 
      if (keypress_map[SDLK_y].first) {
        new_pos.y = diff.y;
      }
      if (keypress_map[SDLK_z].first) {
        new_pos.z = diff.z;
      }

      //ptr->scale(*scene, new_pos);
      if (keypress_map[SDLK_LSHIFT].first) {
        new_pos = vec3(new_pos.x, new_pos.x, new_pos.x);
        ptr->transform_scale_set(new_pos);
      } else {
        ptr->transform_scale_set(new_pos);
      }
      scene->transform_update_global_recursive(ptr);
    }

  } else {
    dof6->getFrameOffsetA().setOrigin(newPivotB);
  }

  //POLL_DEBUG(std::cout, "dist: " << dist);

  printf("newPivotB=%f,%f,%f\n",newPivotB.getX(),newPivotB.getY(),newPivotB.getZ());

}



