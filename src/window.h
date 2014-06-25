#pragma once

#include <SDL2/SDL.h>

#include <iostream>
#include <string>
#include <stdexcept>
#include <functional>

#include "camera.h"
#include "glcontext.h"


using namespace std::placeholders;

class Camera;
class GLcontext;


class Window {
  private:
    bool toggleMouseView;
    bool togglePolygonView;

    void     mouse_cursor_toggle();

  public:
    SDL_Window     *window;
    SDL_GLContext   glSdlContext;
    int             width;
    int             height;

    Window(const int &_width, const int &_height);
    ~Window();

    void     check_error();
    bool     init(const std::string &title);
    bool     keyboard_callback_pressed(SDL_Keysym *keysym, Camera &camera, GLcontext &glcontext);
    void     keyboard_callback_released(SDL_Keysym *keysym, Camera &camera);
    bool     poll_events(Camera &camera, GLcontext &glcontext);
    void     mouse_button_down(SDL_MouseButtonEvent *ev, Camera &camera);
    void     mouse_button_up(SDL_MouseButtonEvent *ev, Camera &camera);
    void     mouse_motion(SDL_MouseMotionEvent *ev, Camera &camera);
    void     term();

    std::function <void (SDL_Keysym *)> custom_keyboard_pressed_callback;
    template <typename Client>
      void keyboard_pressed_callback_set(Client *client, void (Client::*method) (SDL_Keysym *)) 
      {
        custom_keyboard_pressed_callback = std::bind(method, client, _1);
      }
};


