#ifndef UIWINDOW_H
#define UIWINDOW_H

#include <SDL2/SDL.h>

#include <iostream>
#include <string>
#include <stdexcept>


class Window {

public:
  SDL_Window     *window;
  SDL_GLContext   glSdlContext;
  int             width;
  int             height;

   Window(const int &_width, const int &_height);
  ~Window();

  void checkError();
  bool init(const std::string & title);
  void term();

};


#endif
