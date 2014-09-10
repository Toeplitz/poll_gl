#include "scene.h"
#include "raycast.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>


void Raycast::convert_coordinates(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height)
{

  Camera *camera = scene.camera_get();

  /*
  POLL_DEBUG(std::cout, "width / height: " << width << " / " << height);

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

  glm::mat4 InverseProjectionMatrix = camera->transform_perspective_inverse_get();
  glm::mat4 InverseViewMatrix = glm::inverse(camera->transform_view_get());

  glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera/=lRayStart_camera.w;
  glm::vec4 lRayStart_world  = InverseViewMatrix       * lRayStart_camera; lRayStart_world /=lRayStart_world .w;
  glm::vec4 lRayEnd_camera   = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera  /=lRayEnd_camera  .w;
  glm::vec4 lRayEnd_world    = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world   /=lRayEnd_world   .w;
*/

   // Faster way (just one inverse)
   
  /*
   glm::mat4 M = glm::inverse(camera->transform_view_projection_get());
   glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
   glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

  glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
  lRayDir_world = glm::normalize(lRayDir_world);

  glm::vec3 out_origin = glm::vec3(lRayStart_world);
  glm::vec3 out_direction = glm::normalize(lRayDir_world);

  POLL_DEBUG(std::cout, "out origin: " << to_string(out_origin));
  POLL_DEBUG(std::cout, "out direction: " << to_string(out_direction));
*/

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

      glm::vec3 pos = camera->position_get();
      scene.physics_get().ray_pick(pos, ray_wor);


}
