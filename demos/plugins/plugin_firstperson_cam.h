#pragma once

#include "poll.h"
#include "poll_plugin.h"

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
    Node *n;
    Camera *camera;
    float horizontal_angle;
    float vertical_angle;
    float speed;
    float mouse_speed;
    std::vector<Camera_Move> move_queue;
    glm::vec3 up;
    glm::vec3 direction;
    glm::vec3 right;
    glm::vec3 position;
    glm::vec3 target;
    bool mouse_view_toggle;

    void common_fpcamera_defaults_set();
    void common_fpcamera_directions_calc();
    void keyboard_callback_pressed(SDL_Keysym *keysym);
    void keyboard_callback_released(SDL_Keysym *keysym);
    void common_fpcamera_move_add(Camera_Move move);
    void common_fpcamera_move_delete(Camera_Move move);
    void common_fpcamera_mouse_update(int x, int y, int width, int height) ;
    void common_fpcamera_moves_process();
    void mouse_callback_pressed(SDL_MouseButtonEvent *ev);
    void mouse_callback_released(SDL_MouseButtonEvent *ev);
    void mouse_callback_motion(SDL_MouseMotionEvent *ev);
    void custom_callback();

  public:

    Plugin_Firstperson_Camera(Console &console, Scene &scene, Node *node);

};

