#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>


using glm::vec3;


class Scene;
class Node;


struct Raycast_Hitpoint
{
  vec3 world_hitpoint;
  vec3 world_ray;
  vec3 ray_from;
  float length;
  Node *node_ptr;

  Raycast_Hitpoint(vec3 hp, vec3 ray, vec3 f, float l, Node *ptr): 
    world_hitpoint(hp), world_ray(ray), ray_from(f), length(l), node_ptr(ptr)  {}

  bool operator < (const Raycast_Hitpoint &hp) const
  {
    return (length < hp.length);
  }
};


class Raycast
{
  private:

  public:
    vec3                              cast_empty(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height);
    Raycast_Hitpoint                  cast(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height);
    vec3                              get_ray_to(Scene &scene, int x, int y, const int width, const int height);
    vec3                              tutor_cast(Scene &scene, int x, int y, const int width, const int height);

};
