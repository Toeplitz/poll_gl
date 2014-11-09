#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using glm::vec2;
using glm::vec3;
using glm::ivec3;
using glm::vec4;
using glm::ivec4;
using glm::mat4;


#include <iostream>
#include <string>
#include <stdexcept>

#include "config.h"
#include "camera.h"
#include "gldefaults.h"
#include "glshader.h"
#include "poll_plugin.h"
#include "scene.h"
#include "text.h"


class Window;


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
    GLuint gl_fb_shadow;
    GLuint gl_fb_tex_normal;
    GLuint gl_fb_tex_diffuse;
    GLuint gl_fb_tex_depth;
    GLuint gl_fb_tex_shadow;
    GLuint gl_fb_tex_final;
    GLuint gl_fb_vertex_buffers[1];

    GLuint gl_tex_shadow_sampler;

    Window *window_ptr = nullptr;

    bool check_version(const int &major);
    void framebuffer_check_status();
    void framebuffer_create();
    void framebuffer_delete();
    void texture_cubemap_create(Cubemap_Item &item);
    void texture_cubemap_delete(Cubemap &cubemap);
    void texture_create(Texture &texture, GLenum active_texture, GLint filter,
                        GLint wrap, GLint internal_format, GLenum format,
                        const bool unpack_align);

    void draw_geometry_all(Scene &scene, Poll_Plugin_List &plugins);
    void draw_light_all(Scene &scene);
    void draw_light_volume(Node &node, GLshader &shader_stencil, GLshader &shader_light);
    void draw_light_screen(Node &node, GLshader &shader_light);
    void draw_node(Node &node);

    float shadow_jitter_get();
    void shadow_texture_offset_build(int tex_size, int samples_u, int samples_v);
    mat4 shadow_view_projection_get();

    const int shadow_map_width = 512;
    const int shadow_map_height = 512;

  public:
    
    void check_error();
    void draw_mesh(Node &node);
    void draw_scene(Scene &scene, std::vector<Poll_Plugin *> &plugins);
    void draw_text(Node &node);
    void init(Window &window);
    void term();
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
    void uniform_buffers_update_matrices(mat4 &model);
    void uniform_locations_geometry_init(GLshader &shader);
    void uniform_locations_lighting_init(GLshader &shader);
    void uniform_locations_post_proc_init(GLshader &shader);
    void uniform_locations_text_init(GLshader &shader);
    void vertex_buffers_mesh_create(Mesh *mesh, const size_t max_size = 0);
    void vertex_buffers_mesh_update(Mesh *mesh);
    void vertex_buffers_mesh_delete(Mesh *mesh);

};
