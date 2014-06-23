#ifndef UIWINDOW_H
#define UIWINDOW_H

#include <SDL2/SDL.h>

#include <iostream>
#include <string>
#include <stdexcept>


class UIwindow {

public:
  SDL_Window     *window;
  SDL_GLContext   glSdlContext;
  int             width;
  int             height;

   UIwindow(const int &_width, const int &_height);
  ~UIwindow();

  void checkError();
  bool init(const std::string & title);
  void term();

};


#endif
