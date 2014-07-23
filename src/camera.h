#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include "gldefaults.h"
#include "utils.h"


enum Camera_Move { FORWARD, BACKWARD, SIDESTEP_RIGHT, SIDESTEP_LEFT};


class Camera {
  private:
    float fov;
    float initial_fov;
    float horizontalAngle;
    float verticalAngle;
    float speed;
    float mouseSpeed;
    glm::vec3 up;
    glm::vec3 direction;
    glm::vec3 right;
    glm::vec3 position;
    glm::vec3 target;
    std::vector<Camera_Move> move_queue;

    void defaults_set();
    void directions_calc();
    void transform_perspective_create(float width, float height);
    void transform_view_update();
    bool moves_process();
    void processWayPoints(double dt);

  public: 
    glm::mat4 perspective;
    glm::mat4 view;

    Camera(const int width, const int height);
    ~Camera();

    bool mouse_update(int xPos, int yPos, int width, int height);
    void move_add(Camera_Move move);
    void move_delete(Camera_Move move);
    void update(const double dt);
};
