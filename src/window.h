#pragma once

#include <SDL2/SDL_events.h>            // for SDL_MouseButtonEvent, etc
#include <SDL2/SDL_keyboard.h>          // for SDL_Keysym
#include <SDL2/SDL_video.h>             // for SDL_GLContext, SDL_Window
#include <string>                       // for string
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
    GLcontext      glcontext;

    void check_error();
    bool keyboard_callback_pressed(SDL_Keysym *keysym, Camera &camera);
    void keyboard_callback_released(SDL_Keysym *keysym, Camera &camera);
    void mouse_button_down(SDL_MouseButtonEvent *ev, Camera &camera);
    void mouse_button_up(SDL_MouseButtonEvent *ev, Camera &camera);
    void mouse_cursor_toggle();
    void mouse_motion(SDL_MouseMotionEvent *ev, Camera &camera);

    std::function <void (SDL_Keysym *)> custom_keyboard_pressed_callback;
  public:
    bool aabb_view_toggle;
    int  width;
    int  height;

    Window(const int &width, const int &height);
    ~Window();

    bool init(const std::string &title);
    void debug_toggle();
    bool poll_events(Camera &camera);
    void swap_interval_set(const int n);
    void swap();
    void term();

    GLcontext &glcontext_get();

    template <typename Client>
      void keyboard_pressed_callback_set(Client *client, void (Client::*method) (SDL_Keysym *)) 
      {
        custom_keyboard_pressed_callback = std::bind(method, client, _1);
      }

      void keyboard_pressed_callback_set(const std::function<void (SDL_Keysym *)> callback)
      {
        custom_keyboard_pressed_callback = callback;
      }
};


