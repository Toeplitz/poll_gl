#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>


using namespace glm;


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
    void cast(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height);
    vec3 distance_last_cast(const Raycast_Hitpoint *current);


};
