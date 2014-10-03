#include "scene.h"
#include "raycast.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>


vec3 Raycast::cast_empty(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height)
{

  Camera *camera = scene.camera_get();

  float x = (2.0f * viewport_x) / width - 1.0f;
  float y = 1.0f - (2.0f * viewport_y) / height;
  float z = 1.0f;
  vec3 ray_nds = vec3 (x, y, z);

  vec4 ray_clip = vec4 (ray_nds.x, ray_nds.y, -1.0, 1.0);
  vec4 ray_eye = glm::inverse(camera->transform_perspective_get()) * ray_clip;
  ray_eye = vec4 (ray_eye.x, ray_eye.y,  -1.0, 0.0);

  vec3 ray_wor = vec3((glm::inverse (camera->transform_view_get()) * ray_eye));
  ray_wor = glm::normalize (ray_wor);

  return ray_wor;
}


Raycast_Hitpoint *Raycast::cast(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height)
{

  Camera *camera = scene.camera_get();

  float x = (2.0f * viewport_x) / width - 1.0f;
  float y = 1.0f - (2.0f * viewport_y) / height;
  float z = 1.0f;
  vec3 ray_nds = vec3 (x, y, z);

  vec4 ray_clip = vec4 (ray_nds.x, ray_nds.y, -1.0, 1.0);
  vec4 ray_eye = glm::inverse(camera->transform_perspective_get()) * ray_clip;
  ray_eye = vec4 (ray_eye.x, ray_eye.y,  -1.0, 0.0);

  vec3 ray_wor = vec3((glm::inverse (camera->transform_view_get()) * ray_eye));
  ray_wor = glm::normalize (ray_wor);

  vec3 pos = camera->position_get();
  auto hitpoint = scene.physics_get().ray_pick(pos, ray_wor);

  if (!hitpoint) {
    POLL_DEBUG(std::cout, "Hit the background!");
    return nullptr;
  }
  if (!hitpoint->node_ptr) {
    POLL_ERROR(std::cerr, "No node for the clicked object? Should not be possible?");
    return nullptr;
  }

  vec3 distance = distance_last_cast(hitpoint.get());


  POLL_DEBUG(std::cout, "Hit node: " << hitpoint->node_ptr->name_get());
  POLL_DEBUG(std::cout, "Distance from last hit: " << glm::to_string(distance));

  hitpoint->node_ptr->raycast_collide_callback_call(hitpoint->world_hitpoint);

  hitpoint_last = hitpoint;

  return hitpoint.get();
}


vec3 Raycast::distance_last_cast(const Raycast_Hitpoint *current)
{
  if (!hitpoint_last)
    return vec3(0, 0, 0);

  return current->world_hitpoint - hitpoint_last->world_hitpoint;
}

