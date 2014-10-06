#include "scene.h"
#include "raycast.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>


void Raycast_Hitpoint::print()
{
  std::cout << "Raycast_Hitpoint:" << std::endl;
  if (node_ptr)
    std::cout << "Node: " << node_ptr->name_get() << std::endl;

  std::cout << "world_hitpoint: " << glm::to_string(world_hitpoint) << std::endl; 
  std::cout << "world_ray: " << glm::to_string(world_ray) << std::endl; 
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

  return ray_wor;
}


std::shared_ptr<Raycast_Hitpoint> Raycast::cast(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height)
{
  Camera *camera = scene.camera_get();

  //vec3 ray_wor = cast_empty(scene, viewport_x, viewport_y, width, height);
  vec3 ray_wor = get_ray_to(scene, viewport_x, viewport_y, width, height);

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

  POLL_DEBUG(std::cout, "Hit node: " << hitpoint->node_ptr->name_get());

  hitpoint->node_ptr->raycast_collide_callback_call(hitpoint->world_hitpoint);
  hitpoint->world_ray = ray_wor;
  hitpoint->ray_from = pos;

  return hitpoint;
}


vec3 Raycast::get_ray_to(Scene &scene, int x, int y, const int width, const int height)
{
  vec3 camera_pos = scene.camera_get()->position_get();
  vec3 camera_target_pos = scene.camera_get()->target_position_get();

  float top = 1.f;
  float bottom = -1.f;
  float nearPlane = 1.f;
  //float tanFov = (top-bottom)*0.5f / nearPlane;
  //float fov = btScalar(2.0) * btAtan(tanFov);
  float fov = scene.camera_get()->fov_get();

  //btVector3	rayFrom = getCameraPosition();

  btVector3 bt_cam_pos = btVector3(camera_pos.x, camera_pos.y, camera_pos.z);
  btVector3 bt_cam_target_pos = btVector3(camera_target_pos.x, camera_target_pos.y, camera_target_pos.z);
  btVector3 rayFrom = bt_cam_pos;
  btVector3 rayForward = (bt_cam_target_pos - bt_cam_pos);
  rayForward.normalize();
  float farPlane = 500.f;
  rayForward*= farPlane;

  btVector3 rightOffset;
  //btVector3 vertical = m_cameraUp;
  btVector3 vertical(0, 1, 0);

  btVector3 hor;
  hor = rayForward.cross(vertical);
  hor.normalize();
  vertical = hor.cross(rayForward);
  vertical.normalize();

  float tanfov = tanf(0.5f * fov);


  hor *= 2.f * farPlane * tanfov;
  vertical *= 2.f * farPlane * tanfov;

  btScalar aspect;

  aspect = width / (btScalar) height;
  hor*=aspect;

  btVector3 rayToCenter = rayFrom + rayForward;
  btVector3 dHor = hor * 1.f / float(width);
  btVector3 dVert = vertical * 1.f / float(height);

  btVector3 rayTo = rayToCenter - 0.5f * hor + 0.5f * vertical;
  rayTo += btScalar(x) * dHor;
  rayTo -= btScalar(y) * dVert;

  return vec3(rayTo.getX(), rayTo.getY(), rayTo.getZ());
}
