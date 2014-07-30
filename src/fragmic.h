#pragma once

#include <SDL2/SDL_stdinc.h>            // for Uint32
#include <string>                       
#include "camera.h"                     
#include "glcontext.h"                  
#include "glshader.h"                   
#include "physics.h"
#include "scene.h"                      
#include "window.h"                     


class Fragmic {
  private:
    Physics physics;
    GLshader glshader;
    GLshader glshader_deferred_first;
    GLshader glshader_deferred_second;
    GLshader glshader_screen;
    GLshader glshader_stencil;
    Scene scene;
    Window window;

    double       delta_time_get();
    void         draw_g_buffer(const double dt);
    void         draw_standard_post_proc(const double dt);
    void         profile_fps(const double dt);

  public:
    Fragmic(const std::string &title, const int &width, const int &height);
    ~Fragmic();

    void     run();
    void     term();
    Physics &physics_get();
    Scene   &scene_get();
    Window  &window_get();
};


