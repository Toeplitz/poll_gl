#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <iostream>
#include <string>
#include <stdexcept>

#include "config.h"
#include "camera.h"
#include "gldefaults.h"
#include "glshader.h"
#include "scene.h"
#include "text.h"


class GLcontext {
  private:
    GLuint gl_buffer_armature;
    GLuint gl_buffer_config;
    GLuint gl_buffer_globalmatrices;
    GLuint gl_buffer_light;
    GLuint gl_buffer_matrices;
    GLuint gl_buffer_material;
    GLuint gl_buffer_state;

    GLuint gl_fb;
    GLuint gl_fb_tex_normal;
    GLuint gl_fb_tex_diffuse;
    GLuint gl_fb_tex_depth;
    GLuint gl_fb_tex_final;
    GLuint gl_fb_vertex_buffers[1];

    bool check_version(const int &major);
    void texture_cubemap_create(Cubemap_Item &item);
    void texture_cubemap_delete(Cubemap &cubemap);
    void texture_create(Texture &texture, GLenum active_texture, GLint filter,
                        GLint wrap, GLint internal_format, GLenum format,
                        const bool unpack_align);

  public:
    
    void check_error();
    bool init(const int width, const int height);
    void draw_node(Node &node);
    void draw_mesh(Mesh &mesh);
    void draw_text(Node &node);
    void framebuffer_check_status();
    void framebuffer_create(const int width, const int height);
    void framebuffer_delete();
    void framebuffer_draw_scene(Scene &scene, GLshader shader_geometry,  
                                GLshader &shader_stencil, GLshader &shader_light, GLshader &shader_post_proc);
    void framebuffer_node_create(Node &node);
    void texture_delete(Texture &texture);
    void texture_single_channel_create(Texture &texture);
    void texture_materials_create(Material *material);
    void texture_materials_delete(Material *material);
    void uniform_buffers_block_bind(GLshader &shader);
    void uniform_buffers_create(Config &config);
    void uniform_buffers_delete();
    void uniform_buffers_update_armature(const Armature &armature);
    void uniform_buffers_update_camera(Camera &camera);
    void uniform_buffers_update_config(Config &config);
    void uniform_buffers_update_light(Light &light);
    void uniform_buffers_update_material(const Material &material);
    void uniform_buffers_update_matrices(Node &node);
    void uniform_buffers_update_state(Node &node);
    void uniform_locations_geometry_init(GLshader &shader);
    void uniform_locations_lighting_init(GLshader &shader);
    void uniform_locations_post_proc_init(GLshader &shader);
    void uniform_locations_text_init(GLshader &shader);
    void vertex_buffers_mesh_create(Mesh *mesh, const size_t max_size = 0);
    void vertex_buffers_mesh_update(Mesh *mesh);
    void vertex_buffers_mesh_delete(Mesh *mesh);

};
