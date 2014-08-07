#pragma once

#include <SDL2/SDL_stdinc.h>            // for Uint32
#include <string>
#include "console.h"
#include "camera.h"                     
#include "glcontext.h"                  
#include "glshader.h"                   
#include "physics.h"
#include "scene.h"                      
#include "window.h"                     


class Fragmic {
  private:
    Console console;
    Physics physics;
    GLshader glshader_geometry;
    GLshader glshader_light;
    GLshader glshader_stencil;
    Scene scene;
    Window window;

    double       delta_time_get();
    void         draw_standard_post_proc(const double dt);
    void         profile_fps(const double dt);

  public:
    Fragmic(const std::string &title, const int &width, const int &height);
    ~Fragmic();

    void       run();
    void       term();
    Assets    &assets_get();
    Console   &console_get();
    GLcontext &glcontext_get();
    Physics   &physics_get();
    Scene     &scene_get();
    Window    &window_get();
};


