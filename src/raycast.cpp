#include "scene.h"
#include "raycast.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>


void Raycast::convert_coordinates(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height)
{

  Camera *camera = scene.camera_get();

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
