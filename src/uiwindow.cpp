#include "uiwindow.h"

using std::string;
using std::cout;
using std::endl;


UIwindow::UIwindow(const int &_width, const int &_height): 
  width(_width), 
  height(_height)
{
}


UIwindow::~UIwindow()
{
}


void UIwindow::checkError()
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


bool UIwindow::init(const string &title)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cout << "Fragmic error: failed to initialize SDL: " << SDL_GetError() << endl;
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


void UIwindow::term()
{
  std::cout << "Deleting uiwindow (SDL)" << std::endl;
  SDL_DestroyWindow(window);
  SDL_Quit();
}
