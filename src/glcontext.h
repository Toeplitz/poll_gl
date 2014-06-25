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


class GLcontext {
  private:
    GLuint gl_buffer_globalmatrices;
    GLuint gl_buffer_matrices;
    GLuint gl_buffer_armature;
    GLuint gl_buffer_debug;

    bool check_version(const int &major);

  public:
    GLcontext();
    ~GLcontext();

    void check_error();
    void clear();
    void draw(Node &node, bool aabb);
    bool init(const int width, const int height);
    void polygon_mesh_toggle(bool tog);
    void uniform_buffers_init(GLshader &shader);
    void uniform_buffers_update_camera(Camera &camera);
    void uniform_buffers_update_debug(glm::vec4 &data);
    void uniform_buffers_update_mesh(Mesh &mesh, bool aabb);
    void uniform_buffers_update_node(Node &node);
    void vertex_buffers_add(Node &node);

};


#endif
