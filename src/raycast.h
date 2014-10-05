#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>


using glm::vec3;


class Scene;
class Node;


class Raycast_Hitpoint
{
  public:
    Node *node_ptr = nullptr;
    vec3 world_hitpoint = vec3(0.f);
    vec3 world_ray = vec3(0.f);

    vec3 ray_from = vec3(0.f);

    void print();
};


class Raycast
{
  private:

  public:
    vec3                              cast_empty(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height);
    std::shared_ptr<Raycast_Hitpoint> cast(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height);

};
