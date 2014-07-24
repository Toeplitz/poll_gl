#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <functional>


class Camera 
{
  private:
    float fov;
    glm::vec3 up;
    glm::mat4 perspective;
    glm::mat4 view;

    std::function<void ()> update_callback;
  public: 

    Camera();
    ~Camera();

      void update_callback_set(const std::function<void ()> callback)
      {
        update_callback = callback;
      }


    void        transform_perspective_create(const int width, const int height);
    glm::mat4  &transform_perspective_get();
    glm::mat4  &transform_view_get();
    void        transform_view_create(const glm::vec3 position, const glm::vec3 direction);
    void        update();
};
