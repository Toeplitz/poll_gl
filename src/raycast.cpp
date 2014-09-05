#include "scene.h"
#include "raycast.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>


void Raycast::convert_coordinates(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height)
{

  Camera *camera = scene.camera_get();

  // The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
  glm::vec4 lRayStart_NDC(
      ((float) viewport_x / (float)width  - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
      ((float) viewport_y / (float)height - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
      -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
      1.0f
      );

  glm::vec4 lRayEnd_NDC(
      ((float) viewport_x / (float) width - 0.5f) * 2.0f,
      ((float) viewport_y / (float) height - 0.5f) * 2.0f,
      0.0,
      1.0f
      );

  POLL_DEBUG(std::cout, "Ray start: " << to_string(lRayStart_NDC));
  POLL_DEBUG(std::cout, "Ray end: " << to_string(lRayEnd_NDC));

  /*
  glm::mat4 InverseProjectionMatrix = camera->transform_perspective_inverse_get();
  glm::mat4 InverseViewMatrix = glm::inverse(camera->transform_view_get());

  glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera/=lRayStart_camera.w;
  glm::vec4 lRayStart_world  = InverseViewMatrix       * lRayStart_camera; lRayStart_world /=lRayStart_world .w;
  glm::vec4 lRayEnd_camera   = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera  /=lRayEnd_camera  .w;
  glm::vec4 lRayEnd_world    = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world   /=lRayEnd_world   .w;
*/

   // Faster way (just one inverse)
   glm::mat4 M = glm::inverse(camera->transform_view_projection_get());
   glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
   glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

  glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
  lRayDir_world = glm::normalize(lRayDir_world);

  glm::vec3 out_origin = glm::vec3(lRayStart_world);
  glm::vec3 out_direction = glm::normalize(lRayDir_world);

  POLL_DEBUG(std::cout, "out origin: " << to_string(out_origin));
  POLL_DEBUG(std::cout, "out direction: " << to_string(out_direction));
  scene.physics_get().ray_pick(out_origin, out_direction);

  /*
     float x = (2.0f * viewport_x) / width - 1.0f;
     float y = 1.0f - (2.0f * viewport_y) / height;
     float z = 1.0f;
     vec3 ray_nds = vec3 (x, y, z);
     std::cout << to_string(ray_nds) << std::endl;

     vec4 ray_clip = vec4 (ray_nds.x, ray_nds.y, -1.0, 1.0);
     vec4 ray_eye = inverse (camera->transform_perspective_get()) * ray_clip;
     ray_eye = vec4 (ray_eye.x, ray_eye.y,  -1.0, 0.0);
     std::cout << to_string(ray_eye) << std::endl;

     vec3 ray_wor = vec3((inverse (camera->transform_view_get()) * ray_eye));
     ray_wor = normalize (ray_wor);
     std::cout << to_string(ray_wor) << std::endl;

     for (auto &node: scene.mesh_nodes_get()) {

     }
     */

}


bool Raycast::intersect_sphere(vec3 ray_origin_wor, vec3 ray_direction_wor, vec3 sphere_centre_wor, 
    float sphere_radius, float *intersection_distance)
{
  // work out components of quadratic
  vec3 dist_to_sphere = ray_origin_wor - sphere_centre_wor;
  float b = dot (ray_direction_wor, dist_to_sphere);
  float c = dot (dist_to_sphere, dist_to_sphere) -
    sphere_radius * sphere_radius;
  float b_squared_minus_c = b * b - c;
  // check for "imaginary" answer. == ray completely misses sphere
  if (b_squared_minus_c < 0.0f) {
    return false;
  }
  // check for ray hitting twice (in and out of the sphere)
  if (b_squared_minus_c > 0.0f) {
    // get the 2 intersection distances along ray
    float t_a = -b + sqrt (b_squared_minus_c);
    float t_b = -b - sqrt (b_squared_minus_c);
    *intersection_distance = t_b;
    // if behind viewer, throw one or both away
    if (t_a < 0.0) {
      if (t_b < 0.0) {
        return false;
      }
    } else if (t_b < 0.0) {
      *intersection_distance = t_a;
    }

    return true;
  }
  // check for ray hitting once (skimming the surface)
  if (0.0f == b_squared_minus_c) {
    // if behind viewer, throw away
    float t = -b + sqrt (b_squared_minus_c);
    if (t < 0.0f) {
      return false;
    }
    *intersection_distance = t;
    return true;
  }
  // note: could also check if ray origin is inside sphere radius
  return false;

}
