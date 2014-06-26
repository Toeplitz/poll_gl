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
    GLshader shader;
    Scene scene;
    Window window;

    void profile_fps(Uint32 dt);

  public:
    Fragmic(const std::string &title, const int &width, const int &height);
    ~Fragmic();

    void     run();
    void     term();
    Camera  &camera_get();
    Scene   &scene_get();
    Window  &window_get();
};


