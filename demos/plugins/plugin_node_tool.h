#pragma once

#include "poll.h"
#include "poll_plugin.h"

#include <memory>

class Plugin_Node_Tool: public Poll_Plugin
{
  private:
    Console *console = nullptr;
    Scene *scene = nullptr;


  public:
    Plugin_Node_Tool(Console &console, Scene &scene);
    void cb_mouse_pressed(SDL_MouseButtonEvent *ev);
    void cb_mouse_released(SDL_MouseButtonEvent *ev);
    void cb_mouse_motion(SDL_MouseMotionEvent *ev);

};
