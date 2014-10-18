#include "window.h"
#include <SDL2/SDL.h>                   // for SDL_Init, SDL_Quit, etc
#include <SDL2/SDL_error.h>             // for SDL_GetError, etc
#include <SDL2/SDL_keycode.h>           // for ::SDLK_e, ::SDLK_q, etc
#include <SDL2/SDL_mouse.h>             // for SDL_ShowCursor, etc
#include <exception>                    // for exception
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <stdexcept>                    // for runtime_error
#include "glcontext.h"                  
#include "poll_plugin.h"
#include "raycast.h"
#include "utils.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Window::Window()
{
  this->debug = 0;
  this->gamepad = nullptr;
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


bool Window::init(Config &config, Scene &scene, const std::string &title)
{
  const Conf_Global &conf_global = config.conf_global_get();
  width = conf_global.viewport.width;
  height = conf_global.viewport.height;
  this->scene = &scene;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
    POLL_ERROR(std::cerr, "Failed to initialize SDL: " << SDL_GetError());
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, width, height,
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

  if(SDL_NumJoysticks() < 1) {
    POLL_WARN(std::cerr, "No joysticks/gamepads connected!");
  } else {
    gamepad = SDL_JoystickOpen(0);
    if(gamepad == NULL) {
      POLL_WARN(std::cerr, "Unable to open game controller! SDL Error: " << SDL_GetError());
    }
  }

  gl_sdl_context = SDL_GL_CreateContext(window);

  const char *error = SDL_GetError();
  if (*error != '\0') {
    POLL_WARN(std::cerr, "SDL message: " << error);
    SDL_ClearError();
  }
  
  swap_interval_set(config.conf_global_get().viewport.swap_interval);

  return true;
}


float Window::joystick_angle_get(float x, float y, float *radius)
{
  float quadrant_compensation = 0;
  if (x < 0)
    quadrant_compensation = M_PI;
  if (x > 0 && y < 0)
    quadrant_compensation = 2 * M_PI;
  float r = sqrtf(powf(x, 2) + powf(y, 2)) / 32768;
  float angle = atanf((float) y / (float) x) + quadrant_compensation;

  *radius = r;

  return angle;
}


void Window::swap_interval_set(const int n)
{
  // 0 for immediate updates, 1 for updates synchronized with
  // the vertical retrace, -1 for late swap tearing
  SDL_GL_SetSwapInterval(n);
}


void Window::swap()
{
  SDL_GL_SwapWindow(window);
}


void Window::term()
{
  SDL_GL_DeleteContext(gl_sdl_context);
  if (gamepad) {
    SDL_JoystickClose(gamepad);
  }
  SDL_DestroyWindow(window);
  SDL_Quit();
}


bool Window::poll_events(std::vector<Poll_Plugin *> &plugins)
{
  SDL_Event event;

  check_error();

  while (SDL_PollEvent(&event)) {

    switch (event.type) {
      case SDL_QUIT:
        return false;
        break;
      case SDL_KEYUP:
        for (auto plugin : plugins) {
          plugin->cb_keyboard_released(&event.key.keysym);
        }
        keyboard_callback_released(&event.key.keysym);
        break;
      case SDL_KEYDOWN:
        for (auto plugin : plugins) {
          plugin->cb_keyboard_pressed(&event.key.keysym);
        }
        return keyboard_callback_pressed(&event.key.keysym);
      case SDL_JOYAXISMOTION:
        joystick_axis_motion(&event.jaxis);
        break;
      case SDL_JOYBUTTONDOWN:
        joystick_button_pressed(&event.jbutton);
        break;
      case SDL_JOYBUTTONUP:
        joystick_button_released(&event.jbutton);
        break;
      case SDL_MOUSEBUTTONDOWN:
        for (auto plugin : plugins) {
          plugin->cb_mouse_pressed(&event.button);
        }
        mouse_callback_pressed_down(&event.button);
        break;
      case SDL_MOUSEBUTTONUP:
        for (auto plugin : plugins) {
          plugin->cb_mouse_released(&event.button);
        }
        break;
      case SDL_MOUSEMOTION:
        for (auto plugin : plugins) {
          plugin->cb_mouse_motion(&event.motion);
        }
        break;
      default:
        break;
    }
  }

  return true;
}


GLcontext &Window::glcontext_get()
{
  return glcontext;
}


unsigned int Window::height_get() const
{
  return height;
}


unsigned int Window::width_get() const
{
  return width;
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


void Window::check_error()
{
  try {
    const char *sdl_error = SDL_GetError();
    if ('\0' != sdl_error[0]) {
      throw std::runtime_error(std::string("SDL error: ") + sdl_error);
    }
  }
  catch(std::exception & e) {
    POLL_WARN(std::cerr, e.what());
    SDL_ClearError();
  }
}


void Window::joystick_axis_motion(SDL_JoyAxisEvent *ev)
{
  if (custom_joystick_axis_motion_callback)
    custom_joystick_axis_motion_callback(ev);
}


void Window::joystick_button_pressed(SDL_JoyButtonEvent *ev)
{
  if (custom_joystick_pressed_callback)
    custom_joystick_pressed_callback(ev);
}


void Window::joystick_button_released(SDL_JoyButtonEvent *ev)
{
  if (custom_joystick_released_callback)
    custom_joystick_released_callback(ev);
}


bool Window::keyboard_callback_pressed(SDL_Keysym *keysym)
{
  switch (keysym->sym) {
    case SDLK_ESCAPE:
      return false;
      break;
    default:
      break;
  }

  for (auto &func: custom_keyboard_pressed_callback_list) {
    func(keysym);
  }

  if (custom_keyboard_pressed_callback)
    custom_keyboard_pressed_callback(keysym);



  return true;
}


void Window::keyboard_callback_released(SDL_Keysym *keysym)
{
  for (auto &func: custom_keyboard_released_callback_list) {
    func(keysym);
  }
}


void Window::mouse_callback_pressed_down(SDL_MouseButtonEvent *ev)
{
}


void Window::mouse_cursor_center()
{
  SDL_WarpMouseInWindow(window, width / 2, height / 2);
}


void Window::mouse_cursor_toggle()
{
  static bool flagMouseCursor;
  if (flagMouseCursor)
    SDL_ShowCursor(1);
  else
    SDL_ShowCursor(0);
  flagMouseCursor = !flagMouseCursor;
}

