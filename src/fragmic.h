#ifndef FRAGMIC_H
#define FRAGMIC_H

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
#include "buffers.h"
#include "camera.h"
#include "glcontext.h"
#include "glrenderer.h"
#include "glshader.h"
#include "mesh.h"
#include "model.h"
#include "node.h"
#include "scene.h"
#include "texture.h"
#include "transform.h"


using namespace std::placeholders;


class Fragmic {
  private:
    Buffers buffers;
    Camera camera;
    GLcontext context;
    GLshader shader;
    Scene scene;
    UIwindow window;

    bool toggleMouseView;
    bool togglePolygonView;

    void gl_init_uniform_buffers(GLshader &shader);


    void calcFps(Uint32 dt);
    bool keyboardCallbackPressed(SDL_Keysym *keysym);
    void keyboardCallbackReleased(SDL_Keysym *keysym);
    void mouseButtonDown(SDL_MouseButtonEvent *ev);
    void mouseButtonUp(SDL_MouseButtonEvent *ev);
    void mouseMotion(SDL_MouseMotionEvent *ev);
    bool pollEvents();

    std::function <void (SDL_Keysym *)> customKeyboardPressedCallback;


  public:
    Fragmic(const std::string &title, const int &width, const int &height);
    ~Fragmic();

    template <typename Client>
      void setKeyboardPressedCallback(Client *client, void (Client::*method) (SDL_Keysym *)) 
      {
        customKeyboardPressedCallback = std::bind(method, client, _1);
      }

    Scene &getScene()
    {
      return scene;
    }


    Camera &getCamera() 
    {
      return camera;
    }

    void toggleDebug();

    void run();
    void term();

};


#endif
