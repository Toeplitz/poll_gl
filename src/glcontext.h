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
    GLuint gl_buffer_light;
    GLuint gl_vertex_buffers[8];

    GLuint gl_fb;
    GLuint gl_fb_tex;
    GLuint gl_fb_vertex_buffers[1];

    GLuint gl_g_fb;
    GLuint gl_g_fb_tex_normal;
    GLuint gl_g_fb_tex_diffuse;
    GLuint gl_g_fb_tex_depth;
    GLuint gl_g_fb_vertex_buffers[1];

    Node *fb_node;
    Node *fb_g_node;

    bool check_version(const int &major);
    void texture_cubemap_create(Cubemap_Item &item);
    void texture_cubemap_delete(Cubemap &cubemap);
    void texture_create(Texture &texture, GLenum n);
    void texture_delete(Texture &texture);

  public:
    GLcontext();
    ~GLcontext();

    void check_error();
    void clear();
    bool init(const int width, const int height);
    void draw_light(Light *light);
    void draw_node(Node &node);
    void draw_mesh(Mesh &mesh);
    void framebuffer_check_status();
    void framebuffer_create(const int width, const int height);
    void framebuffer_delete();
    void framebuffer_draw_texture(Scene &scene, bool debug);
    void framebuffer_draw_screen();
    void framebuffer_g_create(GLshader &glshader_deferred_second, const int width, const int height);
    void framebuffer_g_draw_first_pass(Scene &scene, GLshader &shader);
    void framebuffer_g_draw_second_pass(const Assets &assets, GLshader &shader_stencil, GLshader &shader);
    void framebuffer_node_create(GLshader &shader, Node &node);
    void polygon_mesh_toggle(bool tog);
    void uniform_buffers_block_bind(GLshader &shader);
    void uniform_buffers_create();
    void uniform_buffers_delete();
    void uniform_buffers_update_armature(const Armature &armature);
    void uniform_buffers_update_camera(Camera &camera);
    void uniform_buffers_update_light(Light &light);
    void uniform_buffers_update_material(const Material &material);
    void uniform_buffers_update_mesh(Mesh &mesh);
    void uniform_buffers_update_state(Node &node);
    void uniform_textures_init(GLshader &shader);
    void vertex_buffers_create(Node &node);
    void vertex_buffers_mesh_create(Mesh *mesh);
    void vertex_buffers_delete(Node &node);
};
