#pragma once

#include <SDL2/SDL_events.h>            // for SDL_MouseButtonEvent, etc
#include <SDL2/SDL_keyboard.h>          // for SDL_Keysym


class Poll_Plugin
{
  public:
    Poll_Plugin();
    ~Poll_Plugin();

    virtual void mouse_callback_pressed(SDL_MouseButtonEvent *ev);
    virtual void mouse_callback_released(SDL_MouseButtonEvent *ev);
    virtual void mouse_callback_motion(SDL_MouseMotionEvent *ev);
    virtual void keyboard_callback_pressed(SDL_Keysym *keysym);
    virtual void keyboard_callback_released(SDL_Keysym *keysym);
    virtual void custom_callback_event(unsigned int type);
    virtual void custom_callback();
};
