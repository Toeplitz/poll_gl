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
#include "camera.h"
#include "glcontext.h"
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
    GLuint gl_buffer_globalmatrices;
    GLuint gl_buffer_matrices;
    GLuint gl_buffer_armature;

    Camera camera;
    GLcontext context;
    GLshader shader;
    Scene scene;
    Window window;

    bool toggleMouseView;
    bool togglePolygonView;

    void     gl_uniform_buffers_init(GLshader &shader, Camera &camera, Scene &scene);
    void     gl_uniform_buffers_update(GLshader &shader, Camera &camera);
    void     gl_uniform_buffers_update_node(GLshader &shader, Node &node);
    void     gl_vertex_buffers_add(Node &node);
    void     gl_draw(Node &node);
    bool     poll_ui_events();
    void     profile_fps(Uint32 dt);
    bool     ui_keyboard_callback_pressed(SDL_Keysym *keysym);
    void     ui_keyboard_callback_released(SDL_Keysym *keysym);
    void     ui_mouse_button_down(SDL_MouseButtonEvent *ev);
    void     ui_mouse_button_up(SDL_MouseButtonEvent *ev);
    void     ui_mouse_motion(SDL_MouseMotionEvent *ev);

    std::function <void (SDL_Keysym *)> ui_custom_keyboard_pressed_callback;
  public:
    Fragmic(const std::string &title, const int &width, const int &height);
    ~Fragmic();

    Camera  &camera_get();
    void     run();
    Scene   &scene_get();
    void     term();

    template <typename Client>
      void ui_set_keyboard_pressed_callback(Client *client, void (Client::*method) (SDL_Keysym *)) 
      {
        ui_custom_keyboard_pressed_callback = std::bind(method, client, _1);
      }

    void toggleDebug();
};


#endif
