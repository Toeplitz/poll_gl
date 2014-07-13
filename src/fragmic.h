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
    Camera camera;
    Physics physics;
    GLshader glshader;
    GLshader glshader_screen;
    Scene scene;
    Window window;

    const double delta_time_get();
    void         profile_fps(const double dt);

  public:
    Fragmic(const std::string &title, const int &width, const int &height);
    ~Fragmic();

    void     run();
    void     term();
    Camera  &camera_get();
    Physics &physics_get();
    Scene   &scene_get();
    Window  &window_get();
};


