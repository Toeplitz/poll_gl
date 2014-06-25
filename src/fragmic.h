#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>

#include <cstddef>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>


#include "assets.h"
#include "armature.h"
#include "bone.h"
#include "camera.h"
#include "glcontext.h"
#include "glshader.h"
#include "mesh.h"
#include "model.h"
#include "node.h"
#include "scene.h"
#include "texture.h"
#include "transform.h"
#include "window.h"




class Fragmic {
  private:
    Camera camera;
    GLcontext glcontext;
    GLshader shader;
    Scene scene;
    Window window;


    void     profile_fps(Uint32 dt);
  public:
    Fragmic(const std::string &title, const int &width, const int &height);
    ~Fragmic();

    void     run();
    void     term();

    Camera  &camera_get();
    Scene   &scene_get();
    Window  &window_get();

    void toggleDebug();
};


