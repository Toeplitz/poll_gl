#ifndef GLCONTEXT_H
#define GLCONTEXT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <stdexcept>

#include "gldefaults.h"
#include "uiwindow.h"


class GLcontext {
private:
  SDL_GLContext glSdlContext;

public:
  GLcontext();
  ~GLcontext();

  bool init(const UIwindow & uiwindow);
  bool checkVersion(const int &major, const int &minor);
  void checkError();
  void clear();
  void setSwapInterval(const int &n);
  void swap(const UIwindow & uiwindow);
  void togglePolygonMesh(bool tog);
  void term();

};


#endif
