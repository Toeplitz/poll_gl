#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;


class Scene;


class Raycast
{
  public:
    void convert_coordinates(Scene &scene, const int viewport_x, const int viewport_y, const int width, const int height);
    bool intersect_sphere(vec3 ray_origin_wor, vec3 ray_direction_wor, vec3 sphere_centre_wor, float sphere_radius, float *intersection_distance);


};
