#pragma once

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
    GLuint gl_buffer_material;
    GLuint gl_buffer_state;
    GLuint gl_vertex_buffers[8];
    GLuint gl_uniform_camera_pos;

    GLuint fbo;
    GLuint rb_depth_buffer;

    bool check_version(const int &major);
    void framebuffer_create();
    void framebuffer_delete();
    void cubemap_texture_create(Cubemap_Item &item);
    void cubemap_texture_delete(Cubemap_Item &item);
    void texture_create(Texture &texture, GLenum n);
    void texture_delete(Texture &texture);

  public:
    GLcontext();
    ~GLcontext();

    void check_error();
    void clear();
    void draw(Node &node);
    bool init(const int width, const int height);
    void polygon_mesh_toggle(bool tog);
    void uniform_buffers_create(GLshader &shader);
    void uniform_buffers_delete();
    void uniform_buffers_update_camera(Camera &camera);
    void uniform_buffers_update_material(Material &material);
    void uniform_buffers_update_mesh(Mesh &mesh);
    void uniform_buffers_update_node(Node &node);
    void uniform_buffers_update_state(Node &node);
    void vertex_buffers_create(Node &node);
    void vertex_buffers_delete(Node &node);
};
