#include "window.h"
#include <SDL2/SDL.h>                   // for SDL_Init, SDL_Quit, etc
#include <SDL2/SDL_error.h>             // for SDL_GetError, etc
#include <SDL2/SDL_keycode.h>           // for ::SDLK_e, ::SDLK_q, etc
#include <SDL2/SDL_mouse.h>             // for SDL_ShowCursor, etc
#include <exception>                    // for exception
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <stdexcept>                    // for runtime_error
#include "camera.h"                     
#include "glcontext.h"                  


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Window::Window(const int &width, const int &height): 
  mouse_view_toggle(false),
  polygon_view_toggle(false),
  glcontext(),
  aabb_view_toggle(false)
{
  this->width = width;
  this->height = height;
}


Window::~Window()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


bool Window::init(const std::string &title)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Window error: failed to initialize SDL: " << SDL_GetError() << std::endl;
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
       

  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                     SDL_WINDOWPOS_CENTERED, width, height,
                     SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  gl_sdl_context = SDL_GL_CreateContext(window);

  const char *error = SDL_GetError();
  if (*error != '\0') {
    std::cout << "GL CONTEXT SDL Error: " << error << std::endl;
    SDL_ClearError();
  }
  return true;
}


void Window::debug_toggle()
{
  static int toggle = 0;
  toggle = !toggle;
  glm::vec4 d(0, 0, 0, toggle);
  glcontext.uniform_buffers_update_debug(d);
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
  std::cout << "Deleting GL context" << std::endl;
  SDL_GL_DeleteContext(gl_sdl_context);
  std::cout << "Deleting uiwindow (SDL)" << std::endl;
  SDL_DestroyWindow(window);
  SDL_Quit();
}


bool Window::poll_events(Camera &camera)
{
  SDL_Event event;

  check_error();
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        return false;
        break;
      case SDL_KEYUP:
        keyboard_callback_released(&event.key.keysym, camera);
        break;
      case SDL_KEYDOWN:
        return keyboard_callback_pressed(&event.key.keysym, camera);
      case SDL_MOUSEBUTTONDOWN:
        mouse_button_down(&event.button, camera);
        break;
      case SDL_MOUSEBUTTONUP:
        mouse_button_up(&event.button, camera);
        break;
      case SDL_MOUSEMOTION:
        mouse_motion(&event.motion, camera);
        break;
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


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


void Window::check_error()
{
  try {
    const char *sdl_error = SDL_GetError();
    if ('\0' != sdl_error[0]) {
      throw std::runtime_error(std::string("SDL error: ") + sdl_error);
      SDL_ClearError();
    }
  }
  catch(std::exception & e) {
    std::cerr << e.what() << std::endl;
  }
}


bool Window::keyboard_callback_pressed(SDL_Keysym *keysym, Camera &camera)
{
  if (custom_keyboard_pressed_callback)
    custom_keyboard_pressed_callback(keysym);

  switch (keysym->sym) {
    case SDLK_ESCAPE:
      return false;
      break;
    case SDLK_w:
      camera.addMove(FORWARD);
      break;
    case SDLK_q:
      camera.addMove(SIDESTEP_LEFT);
      break;
    case SDLK_s:
      camera.addMove(BACKWARD);
      break;
    case SDLK_e:
      camera.addMove(SIDESTEP_RIGHT);
      break;
    case SDLK_m:
      mouse_cursor_toggle();
      mouse_view_toggle = !mouse_view_toggle;
      break;
    case SDLK_p:
      glcontext.polygon_mesh_toggle(polygon_view_toggle);
      polygon_view_toggle = !polygon_view_toggle;
      break;
    case SDLK_a:
      aabb_view_toggle = !aabb_view_toggle;
      break;
    default:
      break;
  }

  return true;
}


void Window::keyboard_callback_released(SDL_Keysym *keysym, Camera &camera)
{

  switch (keysym->sym) {
    case SDLK_w:
      camera.deleteMove(FORWARD);
      break;
    case SDLK_q:
      camera.deleteMove(SIDESTEP_LEFT);
      break;
    case SDLK_s:
      camera.deleteMove(BACKWARD);
      break;
    case SDLK_e:
      camera.deleteMove(SIDESTEP_RIGHT);
      break;
    default:
      break;
  }

}


void Window::mouse_button_down(SDL_MouseButtonEvent *ev, Camera &camera)
{
  if (ev->button != 3 || !mouse_view_toggle)
    return;
  camera.addMove(FORWARD);
}


void Window::mouse_button_up(SDL_MouseButtonEvent *ev, Camera &camera)
{
  if (ev->button != 3 || !mouse_view_toggle)
    return;
  camera.deleteMove(FORWARD);
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


void Window::mouse_motion(SDL_MouseMotionEvent *ev, Camera &camera)
{
  static int last_x, last_y;

  if (!mouse_view_toggle)
    return;

  if (!last_x)
    last_x = ev->x;
  if (!last_y)
    last_y = ev->y;

  camera.mouseViewUpdate(ev->x, ev->y, width, height);
  SDL_WarpMouseInWindow(window, width / 2, height / 2);

  last_x = ev->x;
  last_y = ev->y;
}
