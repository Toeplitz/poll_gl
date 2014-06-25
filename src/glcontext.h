#ifndef GLCONTEXT_H
#define GLCONTEXT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <stdexcept>

#include "camera.h"
#include "gldefaults.h"
#include "glshader.h"
#include "scene.h"
#include "window.h"

class Window;
class Camera;

class GLcontext {
  private:
    GLuint gl_buffer_globalmatrices;
    GLuint gl_buffer_matrices;
    GLuint gl_buffer_armature;
    SDL_GLContext glSdlContext;

    bool check_version(const int &major, const int &minor);

  public:
    GLcontext();
    ~GLcontext();

    void check_error();
    void clear();
    void draw(Node &node);
    bool init(const Window &window);
    void polygon_mesh_toggle(bool tog);
    void swap_interval_set(const int &n);
    void swap(const Window &uiwindow);
    void term();
    void uniform_buffers_init(GLshader &shader, Camera &camera, Scene &scene);
    void uniform_buffers_update(GLshader &shader, Camera &camera);
    void uniform_buffers_update_node(GLshader &shader, Node &node);
    void vertex_buffers_add(Node &node);

};


#endif
