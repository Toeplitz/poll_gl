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
  Node *node_ptr;
  vec3 world_hitpoint;
};


class Raycast
{
  private:

    std::shared_ptr<Raycast_Hitpoint> hitpoint_last;



  public:
    vec3              cast_empty(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height);
    Raycast_Hitpoint *cast(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height);
    vec3              distance_last_cast(const Raycast_Hitpoint *current);


};
