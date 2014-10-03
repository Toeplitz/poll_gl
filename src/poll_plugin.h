#pragma once

#include <SDL2/SDL_events.h>            // for SDL_MouseButtonEvent, etc
#include <SDL2/SDL_keyboard.h>          // for SDL_Keysym
#include <vector>

class Node;


class Poll_Plugin
{
  public:
    Poll_Plugin();
    ~Poll_Plugin();

    /* This is called every loop, straight after the SDL event polling */
    virtual void cb_custom();

    /* This is called after the lighting pass, but before the final screen quad */
    virtual void cb_custom_draw();

    /* This is called after a light is created */
    virtual void cb_light_create(Node *node);

    /* This is called after a node is created */
    virtual void cb_node_create(Node *node);

    virtual void cb_mouse_pressed(SDL_MouseButtonEvent *ev);
    virtual void cb_mouse_released(SDL_MouseButtonEvent *ev);
    virtual void cb_mouse_motion(SDL_MouseMotionEvent *ev);
    virtual void cb_keyboard_pressed(SDL_Keysym *keysym);
    virtual void cb_keyboard_released(SDL_Keysym *keysym);

};

typedef std::vector<Poll_Plugin *> Poll_Plugin_List;
