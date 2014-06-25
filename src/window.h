#pragma once

#include <SDL2/SDL.h>

#include <iostream>
#include <string>
#include <stdexcept>
#include <functional>

#include "camera.h"
#include "glcontext.h"


using namespace std::placeholders;


class Window {
  private:
    bool           mouse_view_toggle;
    bool           polygon_view_toggle;
    SDL_Window    *window;
    SDL_GLContext  gl_sdl_context;

    void check_error();
    bool keyboard_callback_pressed(SDL_Keysym *keysym, Camera &camera, GLcontext &glcontext);
    void keyboard_callback_released(SDL_Keysym *keysym, Camera &camera);
    void mouse_button_down(SDL_MouseButtonEvent *ev, Camera &camera);
    void mouse_button_up(SDL_MouseButtonEvent *ev, Camera &camera);
    void mouse_cursor_toggle();
    void mouse_motion(SDL_MouseMotionEvent *ev, Camera &camera);

    std::function <void (SDL_Keysym *)> custom_keyboard_pressed_callback;
  public:
    int width;
    int height;

    Window(const int &width, const int &height);
    ~Window();

    bool init(const std::string &title);
    bool poll_events(Camera &camera, GLcontext &glcontext);
    void swap_interval_set(const int n);
    void swap();
    void term();

    template <typename Client>
      void keyboard_pressed_callback_set(Client *client, void (Client::*method) (SDL_Keysym *)) 
      {
        custom_keyboard_pressed_callback = std::bind(method, client, _1);
      }
};


