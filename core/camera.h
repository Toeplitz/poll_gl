#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <functional>
#include "config.h"


class Camera 
{
  private:
    float fov = 45.f;
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::mat4 perspective;
    glm::mat4 inv_perspective;
    glm::mat4 view;
    glm::mat4 view_projection = glm::mat4(1.f);
    glm::vec3 position;
    glm::vec3 target_position;
    int width;
    int height;

  public: 

    void               fov_set(const float val);
    float              fov_get();
    const glm::vec3   &position_get();
    const glm::vec3   &target_position_get();
    void               transform_perspective_create(const int width, const int height);
    glm::mat4         &transform_perspective_get();
    glm::mat4         &transform_perspective_inverse_get();
    glm::mat4         &transform_view_get();
    glm::mat4         &transform_view_projection_get();
    glm::mat4         &transform_view_projection_update();
    void               transform_view_create(const glm::vec3 position, const glm::vec3 direction);
    void               update();
};
