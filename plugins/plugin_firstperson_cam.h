#pragma once

#include "poll.h"
#include "poll_plugin.h"

#include <QMouseEvent>
#include <QKeyEvent>

class Console;
class Scene;
class Window;


class Plugin_Firstperson_Camera: public Poll_Plugin
{
  
  private:
    Console *console = nullptr;
    Scene *scene = nullptr;
    Window *window = nullptr;

    enum Camera_Move { FORWARD, BACKWARD, SIDESTEP_RIGHT, SIDESTEP_LEFT};
    Node *n = nullptr;
    Camera *camera = nullptr;
    float horizontal_angle = 0.f;
    float vertical_angle = 0.f;
    float speed = 0.f;
    float mouse_speed = 0.f;
    std::vector<Camera_Move> move_queue;
    glm::vec3 up;
    glm::vec3 direction;
    glm::vec3 right;
    glm::vec3 position;
    glm::vec3 target;
    bool mouse_view_toggle = false;

    void cb_keyboard_pressed(QKeyEvent *e);
    void cb_keyboard_released(QKeyEvent *e);
    /*
    void cb_mouse_pressed(SDL_MouseButtonEvent *ev);
    void cb_mouse_released(SDL_MouseButtonEvent *ev);
    void cb_mouse_motion(SDL_MouseMotionEvent *ev);
    */
    void cb_custom(const float dt);
    void common_fpcamera_defaults_set();
    void common_fpcamera_directions_calc();
    void common_fpcamera_move_add(Camera_Move move);
    void common_fpcamera_move_delete(Camera_Move move);
    void common_fpcamera_mouse_update(int x, int y, int width, int height) ;
    void common_fpcamera_moves_process(const float dt);

  public:

    Plugin_Firstperson_Camera(Scene &scene, Node *node);

};

