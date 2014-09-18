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
    virtual void custom_callback();

    /* This is called after the lighting pass, but before the final screen quad */
    virtual void custom_callback_draw();

    /* This is called after a light is created */
    virtual void light_callback_create(Node *node);

    virtual void mouse_callback_pressed(SDL_MouseButtonEvent *ev);
    virtual void mouse_callback_released(SDL_MouseButtonEvent *ev);
    virtual void mouse_callback_motion(SDL_MouseMotionEvent *ev);
    virtual void keyboard_callback_pressed(SDL_Keysym *keysym);
    virtual void keyboard_callback_released(SDL_Keysym *keysym);

};

typedef std::vector<Poll_Plugin *> Poll_Plugin_List;
