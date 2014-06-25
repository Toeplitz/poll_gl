#include "window.h"

using std::string;
using std::cout;
using std::endl;


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


void Window::mouse_cursor_toggle()
{
  static bool flagMouseCursor;
  if (flagMouseCursor)
    SDL_ShowCursor(1);
  else
    SDL_ShowCursor(0);
  flagMouseCursor = !flagMouseCursor;
}


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Window::Window(const int &_width, const int &_height): 
  toggleMouseView(false),
  togglePolygonView(false),
  width(_width), 
  height(_height)
{
}


Window::~Window()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
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


bool Window::init(const string &title)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cout << "Window error: failed to initialize SDL: " << SDL_GetError() << endl;
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
  return true;
}


void Window::term()
{
  std::cout << "Deleting uiwindow (SDL)" << std::endl;
  SDL_DestroyWindow(window);
  SDL_Quit();
}


bool Window::poll_events(Camera &camera, GLcontext &glcontext)
{
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        return false;
        break;
      case SDL_KEYUP:
        keyboard_callback_released(&event.key.keysym, camera);
        break;
      case SDL_KEYDOWN:
        return keyboard_callback_pressed(&event.key.keysym, camera, glcontext);
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


bool Window::keyboard_callback_pressed(SDL_Keysym *keysym, Camera &camera, GLcontext &glcontext)
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
      toggleMouseView = !toggleMouseView;
      break;
    case SDLK_p:
      glcontext.polygon_mesh_toggle(togglePolygonView);
      togglePolygonView = !togglePolygonView;
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
  if (ev->button != 3 || !toggleMouseView)
    return;
  camera.addMove(FORWARD);
}


void Window::mouse_button_up(SDL_MouseButtonEvent *ev, Camera &camera)
{
  if (ev->button != 3 || !toggleMouseView)
    return;
  camera.deleteMove(FORWARD);
}


void Window::mouse_motion(SDL_MouseMotionEvent *ev, Camera &camera)
{
  static int last_x, last_y;

  if (!toggleMouseView)
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
