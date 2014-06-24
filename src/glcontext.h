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
#include "window.h"


class GLcontext {
  private:
    SDL_GLContext glSdlContext;

  public:
    GLcontext();
    ~GLcontext();

    bool init(const Window & uiwindow);
    bool check_version(const int &major, const int &minor);
    void check_error();
    void clear();
    void swap_interval_set(const int &n);
    void swap(const Window & uiwindow);
    void polygon_mesh_toggle(bool tog);
    void term();

};


#endif
