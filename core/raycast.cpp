#include "scene.h"
#include "raycast.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>


bool Raycast_Hitpoint::operator < (const Raycast_Hitpoint &hp) const
{
  Node *node = hp.node_ptr;

  if (node_ptr->raycast_priority_get() < node->raycast_priority_get())
    return false;

  if (node_ptr->raycast_priority_get() == node->raycast_priority_get())
    return (length < hp.length);

  return true;
}


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

  vec3 ray_wor = vec3((glm::inverse(camera->transform_view_get()) * ray_eye));
  ray_wor = glm::normalize(ray_wor);

  return ray_wor * 500.f;
}


Raycast_Hitpoint Raycast::cast(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height)
{
  Camera *camera = scene.camera_get();

 // vec3 ray_wor = cast_empty(scene, viewport_x, viewport_y, width, height);
  vec3 ray_wor = get_ray_to(scene, viewport_x, viewport_y, width, height);

  vec3 pos = camera->position_get();
  auto hitpoint = scene.physics_get().ray_pick(pos, ray_wor);

  if (!hitpoint.length) {
    POLL_DEBUG(std::cout, "Hit the background!");
    return hitpoint;
  }

  if (!hitpoint.node_ptr) {
    POLL_ERROR(std::cerr, "No node for the clicked object? Should not be possible?");
    return hitpoint;
  }

  POLL_DEBUG(std::cout, "Hit node: " << hitpoint.node_ptr->name_get());

  hitpoint.node_ptr->callback_raycast_collide_call(hitpoint.world_hitpoint);
  hitpoint.world_ray = ray_wor;
  hitpoint.ray_from = pos;

  return hitpoint;
}


vec3 Raycast::get_ray_to(Scene &scene, int x, int y, const int width, const int height)
{
  vec3 camera_pos = scene.camera_get()->position_get();
  vec3 camera_target_pos = scene.camera_get()->target_position_get();
  float fov = scene.camera_get()->fov_get();

  vec3 rayFrom = camera_pos;
  vec3 rayForward = (camera_target_pos - camera_pos);
  rayForward = glm::normalize(rayForward);
  float farPlane = 500.f;
  rayForward *= farPlane;

  vec3 vertical(0, 1, 0);

  vec3 hor = glm::cross(rayForward, vertical);
  hor = glm::normalize(hor);
  vertical = glm::cross(hor, rayForward);
  vertical = glm::normalize(vertical);

  float tanfov = tanf(0.5f * fov);


  hor *= 2.f * farPlane * tanfov;
  vertical *= 2.f * farPlane * tanfov;

  float aspect;
  aspect = (float) width / (float) height;
  hor *= aspect;

  vec3 rayToCenter = rayFrom + rayForward;
  vec3 dHor = hor * 1.f / float(width);
  vec3 dVert = vertical * 1.f / float(height);

  vec3 rayTo = rayToCenter - 0.5f * hor + 0.5f * vertical;
  rayTo += btScalar(x) * dHor;
  rayTo -= btScalar(y) * dVert;

  return rayTo;
}


