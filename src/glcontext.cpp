#include "poll.h"
#include "glcontext.h"
#include "material.h"
#include "node.h"
#include "utils.h"
#include "window.h"
#include <glm/gtx/string_cast.hpp>


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void GLcontext::check_error()
{
  GLenum gl_error = glGetError();
  if (GL_NO_ERROR != gl_error) {
    std::cout << "OpenGL error: " << gluErrorString(gl_error) << std::endl;   
  }
}


void GLcontext::draw_mesh(Node &node)
{
  NODE_VALIDATE(node);
  Mesh &mesh = *node.mesh_get();

  GL_ASSERT(glBindVertexArray(mesh.gl_vao));
  GLsizei count = (GLsizei) mesh.num_indices_get();
  if (count <= 0) {
    GL_ASSERT(glDrawArrays(mesh.mode, 0, mesh.num_vertices_get()));
  } else {
    GL_ASSERT(glDrawElements(mesh.mode, count, GL_UNSIGNED_SHORT, 0));
  }

  node.callback_draw_call();
}


void GLcontext::draw_scene(Scene &scene, Poll_Plugin_List &plugins)
{
  GL_ASSERT(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl_fb));
  GL_ASSERT(glDrawBuffer(GL_COLOR_ATTACHMENT2));
  GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT));

  draw_geometry_all(scene);
  draw_light_all(scene);

  GL_ASSERT(glEnable(GL_DEPTH_TEST));
  for (auto plugin : plugins) {
    plugin->cb_custom_draw();
  }
  GL_ASSERT(glDisable(GL_DEPTH_TEST));

  /* Step physics simulation */
  scene.physics_get().step(scene, 1/60);

  GL_ASSERT(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
  GL_ASSERT(glBindFramebuffer(GL_READ_FRAMEBUFFER, gl_fb));
  GL_ASSERT(glReadBuffer(GL_COLOR_ATTACHMENT2));

  {
    Assets &assets = scene.assets_get();
    Stock_Shaders &shader = assets.stock_shaders_get();
    Node *node = assets.stock_nodes_get().screen_quad_get();
    shader.screen_post_proc.use();
    GL_ASSERT(glActiveTexture(GL_TEXTURE0));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, gl_fb_tex_final));
    GL_ASSERT(glActiveTexture(GL_TEXTURE1));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, gl_fb_tex_shadow));
    draw_mesh(*node);
  }

  {
    const int dest_width = 200;
    const int dest_height = 200;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gl_fb);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, scene.window_get().width_get(), scene.window_get().height_get(),
        0, 0, dest_width, dest_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glBlitFramebuffer(0, 0, scene.window_get().width_get(), scene.window_get().height_get(),
        dest_width * 2, 0, dest_width * 3, 200, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glBlitFramebuffer(0, 0, scene.window_get().width_get(), scene.window_get().height_get(),
        dest_width * 5, 0, dest_width * 6, 200, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, 0);
  }

}


void GLcontext::draw_text(Node &node)
{
  NODE_VALIDATE(node);
  Font *font = nullptr;
  Mesh *mesh = node.mesh_get();
  Text *text = node.text_get();

  if (!text) {
    POLL_ERROR(std::cerr, "No text attached to node: '" << node.name_get() << "'");
    return;
  }
  if (!mesh) {
    POLL_ERROR(std::cerr, "No mesh attached to node: '" << node.name_get() << "'");
    return;
  }

  font = text->font_get();
  if (!font) {
    POLL_ERROR(std::cerr, "No font attached to text object");
    return;
  }

  Texture &texture = font->texture_get();

  GL_ASSERT(glEnable(GL_BLEND));
  GL_ASSERT(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  GL_ASSERT(glActiveTexture(GL_TEXTURE0));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, texture.gl_texture));
  draw_mesh(node);
  GL_ASSERT(glDisable(GL_BLEND));
}



void GLcontext::init(Window &window)
{
  this->window_ptr = &window;

  glewExperimental= GL_TRUE;
  if (glewInit() != GLEW_OK) {
    POLL_ERROR(std::cerr, "Failed to initalize GLEW, exiting ...");
    exit(-1);
  }
  check_error();
  check_version(3);

  GL_ASSERT(glViewport(0, 0, window.width_get(), window.height_get()));
  GL_ASSERT(glCullFace(GL_BACK));
  GL_ASSERT(glEnable(GL_CULL_FACE));
  GL_ASSERT(glEnable(GL_DEPTH_TEST));

  framebuffer_create();
  shadow_texture_offset_build(8, 4, 8);
}



void GLcontext::term()
{
  uniform_buffers_delete();
}


void GLcontext::texture_delete(Texture &texture)
{
  if (!glIsTexture(texture.gl_texture)) return;
  GL_ASSERT(glDeleteTextures(1, &texture.gl_texture));
}


void GLcontext::texture_materials_create(Material *material)
{
  GLint filter = GL_LINEAR;
  GLenum wrap = GL_REPEAT;
  GLint internal_format = GL_RGB;
  GLenum format = GL_RGB;
  bool unpack_align = true;

  if (!material)
    return;

  if (material->diffuse) {
    texture_create(*material->diffuse, GL_TEXTURE0, filter, wrap, internal_format, format, unpack_align);
  }
  if (material->normal) {
    texture_create(*material->normal, GL_TEXTURE1, filter, wrap, internal_format, format, unpack_align);
  }
  if (material->specular) {
    texture_create(*material->specular, GL_TEXTURE2, filter, wrap, internal_format, format, unpack_align);
  }
  if (material->cubemap) {
    if (!glIsTexture(material->cubemap->gl_texture)) {
      GL_ASSERT(glGenTextures(1,  &material->cubemap->gl_texture));
      GL_ASSERT(glActiveTexture(GL_TEXTURE3));
      GL_ASSERT(glBindTexture(GL_TEXTURE_CUBE_MAP, material->cubemap->gl_texture));
      texture_cubemap_create(material->cubemap->front);
      texture_cubemap_create(material->cubemap->back);
      texture_cubemap_create(material->cubemap->top);
      texture_cubemap_create(material->cubemap->bottom);
      texture_cubemap_create(material->cubemap->left);
      texture_cubemap_create(material->cubemap->right);
    }
  }
}


void GLcontext::texture_materials_delete(Material *material)
{
  if (!material)
    return;

  if (material->diffuse) {
    texture_delete(*material->diffuse);
  }
  if (material->normal) {
    texture_delete(*material->normal);
  }
  if (material->specular) {
    texture_delete(*material->specular);
  }
  if (material->cubemap) {
    texture_cubemap_delete(*material->cubemap);
  }

}


void GLcontext::texture_single_channel_create(Texture &texture)
{
  GLint filter = GL_LINEAR;
  GLenum wrap = GL_NONE;
  GLint internal_format = GL_RED;
  GLenum format = GL_RED;
  bool unpack_align = false;

  texture_create(texture, GL_TEXTURE0, filter, wrap, internal_format, format, unpack_align);
}


void GLcontext::uniform_buffers_block_bind(GLshader &shader)
{
  GLuint block_index;
  GLuint bind_index;

  shader.use();

  for (auto &name : shader.block_names_get()) {
    bind_index = uniform_buffer_map.at(name);
    POLL_DEBUG(std::cout, "Shader: " << shader.filename_get() << "Binding " << name << " at index: " << bind_index);
    block_index = shader.get_block_index(name);
    GL_ASSERT(glUniformBlockBinding(shader.program_get(), block_index, bind_index));
  }
}


void GLcontext::uniform_buffers_create(Config &config)
{
  GLenum target = GL_UNIFORM_BUFFER;
  GLuint bind_index;

  {
    mat4 matrix[4];
    bind_index = UB_GLOBALMATRICES;
    GL_ASSERT(glGenBuffers(1, &gl_buffer_globalmatrices));
    GL_ASSERT(glBindBuffer(target, gl_buffer_globalmatrices));
    GL_ASSERT(glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW));
    GL_ASSERT(glBindBufferBase(target, bind_index, gl_buffer_globalmatrices));
    GL_ASSERT(glBindBuffer(target, 0));
  }

  {
    mat4 matrix;
    bind_index = UB_MATRICES;
    GL_ASSERT(glGenBuffers(1, &gl_buffer_matrices));
    GL_ASSERT(glBindBuffer(target, gl_buffer_matrices));
    GL_ASSERT(glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW));
    GL_ASSERT(glBindBufferBase(target, bind_index, gl_buffer_matrices));
    GL_ASSERT(glBindBuffer(target, 0));
  }

  {
    mat4 matrix[64];
    bind_index = UB_ARMATURE;
    GL_ASSERT(glGenBuffers(1, &gl_buffer_armature));
    GL_ASSERT(glBindBuffer(target, gl_buffer_armature));
    GL_ASSERT(glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW));
    GL_ASSERT(glBindBufferBase(target, bind_index, gl_buffer_armature));
    GL_ASSERT(glBindBuffer(target, 0));
  }

  {
    Material_Properties properties;
    properties.Ka = vec4(1.f, 0.f, 0.f, 1.f);
    properties.Kd = vec4(0.f, 1.f, 0.f, 1.f);
    properties.Ks = vec4(0.f, 0.f, 1.f, 1.f);
    properties.shininess = 0.f;

    bind_index = UB_MATERIAL;
    GL_ASSERT(glGenBuffers(1, &gl_buffer_material));
    GL_ASSERT(glBindBuffer(target, gl_buffer_material));
    GL_ASSERT(glBufferData(target, sizeof(properties), &properties, GL_STREAM_DRAW));
    GL_ASSERT(glBindBufferBase(target, bind_index, gl_buffer_material));
    GL_ASSERT(glBindBuffer(target, 0));
  }

  {
    Node_State state;
    state.animated = false;
    state.debug = false;
    state.diffuse = false;
    state.diffuse_normal = false;
    state.diffuse_specular_normal = false;
    state.cubemap_reflect = false;
    state.cubemap_skybox = false;
    state.standard = false;

    bind_index = UB_STATE;
    GL_ASSERT(glGenBuffers(1, &gl_buffer_state));
    GL_ASSERT(glBindBuffer(target, gl_buffer_state));
    GL_ASSERT(glBufferData(target, sizeof(state), &state, GL_STREAM_DRAW));
    GL_ASSERT(glBindBufferBase(target, bind_index, gl_buffer_state));
    GL_ASSERT(glBindBuffer(target, 0));
  }

  {
    Light_Properties properties;

    bind_index = UB_LIGHT;
    GL_ASSERT(glGenBuffers(1, &gl_buffer_light));
    GL_ASSERT(glBindBuffer(target, gl_buffer_light));
    GL_ASSERT(glBufferData(target, sizeof(properties), &properties, GL_STREAM_DRAW));
    GL_ASSERT(glBindBufferBase(target, bind_index, gl_buffer_light));
    GL_ASSERT(glBindBuffer(target, 0));
  }

  {

    struct {
      vec4 ssoa;
      ivec4 viewport;
    } p;

    const Conf_Global &conf_global = config.conf_global_get();

    p.ssoa.x = conf_global.ssao.distance_threshold;
    p.ssoa.y = conf_global.ssao.filter_radius;
    p.ssoa.z = conf_global.ssao.sample_count;

    p.viewport.x = conf_global.viewport.width;
    p.viewport.y = conf_global.viewport.height;

    bind_index = UB_CONFIG;
    GL_ASSERT(glGenBuffers(1, &gl_buffer_config));
    GL_ASSERT(glBindBuffer(target, gl_buffer_config));
    GL_ASSERT(glBufferData(target, sizeof(p), &p, GL_STREAM_DRAW));
    GL_ASSERT(glBindBufferBase(target, bind_index, gl_buffer_config));
    GL_ASSERT(glBindBuffer(target, 0));
  }
}


void GLcontext::uniform_buffers_delete()
{
  GL_ASSERT(glDeleteBuffers(1, &gl_buffer_globalmatrices));
  GL_ASSERT(glDeleteBuffers(1, &gl_buffer_matrices));
  GL_ASSERT(glDeleteBuffers(1, &gl_buffer_armature));
  GL_ASSERT(glDeleteBuffers(1, &gl_buffer_material));
  GL_ASSERT(glDeleteBuffers(1, &gl_buffer_state));
  GL_ASSERT(glDeleteBuffers(1, &gl_buffer_light));
  GL_ASSERT(glDeleteBuffers(1, &gl_buffer_config));
}


void GLcontext::uniform_buffers_update_armature(const Armature &armature)
{
  GLenum target = GL_UNIFORM_BUFFER;
  GL_ASSERT(glBindBuffer(target, gl_buffer_armature));
  GL_ASSERT(glBufferSubData(target, 0, armature.skinning_matrices.size() * sizeof(armature.skinning_matrices[0]), 
        armature.skinning_matrices.data()));
  GL_ASSERT(glBindBuffer(target, 0));
}


void GLcontext::uniform_buffers_update_camera(Camera &camera)
{
  mat4 data[4];
  data[0] = camera.transform_perspective_get();
  data[1] = camera.transform_perspective_inverse_get();
  data[2] = camera.transform_view_get();
  data[3] = camera.transform_view_projection_get();

  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, gl_buffer_globalmatrices));
  GL_ASSERT(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data));
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}


void GLcontext::uniform_buffers_update_config(Config &config)
{
  struct {
    vec4 ssoa;
    ivec4 viewport;
  } p;

  const Conf_Global &conf_global = config.conf_global_get();

  p.ssoa.x = conf_global.ssao.distance_threshold;
  p.ssoa.y = conf_global.ssao.filter_radius;
  p.ssoa.z = conf_global.ssao.sample_count;

  p.viewport.x = conf_global.viewport.width;
  p.viewport.y = conf_global.viewport.height;

  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, gl_buffer_config));
  GL_ASSERT(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(p), &p));
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}


void GLcontext::uniform_buffers_update_light(Light &light)
{
  Light_Properties properties = light.properties_get();

  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, gl_buffer_light));
  GL_ASSERT(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(properties), &properties));
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}


void GLcontext::uniform_buffers_update_material(const Material &material)
{
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, gl_buffer_material));
  GL_ASSERT(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(material.properties), &material.properties));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, 0));

  if (material.diffuse) {
    GL_ASSERT(glActiveTexture(GL_TEXTURE0));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, material.diffuse->gl_texture));
  }
  if (material.normal) {
    GL_ASSERT(glActiveTexture(GL_TEXTURE1));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, material.normal->gl_texture));
  }
  if (material.specular) {
    GL_ASSERT(glActiveTexture(GL_TEXTURE2));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, material.specular->gl_texture));
  }
  if (material.cubemap) {
    GL_ASSERT(glActiveTexture(GL_TEXTURE3));
    GL_ASSERT(glBindTexture(GL_TEXTURE_CUBE_MAP, material.cubemap->gl_texture));
  }
}


void GLcontext::uniform_buffers_update_matrices(Node &node)
{
  mat4 m;
  m = node.transform_global_get();
  uniform_buffers_update_matrices(m);
}


void GLcontext::uniform_buffers_update_matrices(mat4 &model)
{
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, gl_buffer_matrices));
  GL_ASSERT(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(model), &model));
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}


void GLcontext::uniform_buffers_update_state(Node &node)
{
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, gl_buffer_state));
  GL_ASSERT(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(node.state_get()), &node.state_get()));
  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}


void GLcontext::uniform_locations_lighting_init(GLshader &shader)
{
  GLint location;
  GLuint program = shader.program_get();

  shader.use();

  location = glGetUniformLocation(program, "normal_tex");
  GL_ASSERT(glUniform1i(location, 0));
  location = glGetUniformLocation(program, "diffuse_tex");
  GL_ASSERT(glUniform1i(location, 1));
  location = glGetUniformLocation(program, "depth_tex");
  GL_ASSERT(glUniform1i(location, 2));
  location = glGetUniformLocation(program, "shadow_tex");
  GL_ASSERT(glUniform1i(location, 3));
  location = glGetUniformLocation(program, "offset_tex");
  GL_ASSERT(glUniform1i(location, 4));
}


void GLcontext::uniform_locations_geometry_init(GLshader &shader)
{
  GLuint program = shader.program_get();
  GLint location;

  shader.use();

  location = glGetUniformLocation(program, "diffuse_texture");
  GL_ASSERT(glUniform1i(location, 0));
  location = glGetUniformLocation(program, "normal_texture");
  GL_ASSERT(glUniform1i(location, 1));
  location = glGetUniformLocation(program, "specular_texture");
  GL_ASSERT(glUniform1i(location, 2));
}


void GLcontext::uniform_locations_post_proc_init(GLshader &shader)
{
  GLuint program = shader.program_get();
  GLint location;

  shader.use();

  location = glGetUniformLocation(program, "tex");
  GL_ASSERT(glUniform1i(location, 0));

  location = glGetUniformLocation(program, "shadow_tex");
  GL_ASSERT(glUniform1i(location, 1));
}


void GLcontext::uniform_locations_text_init(GLshader &shader)
{
  GLint location;
  GLuint program = shader.program_get();

  shader.use();

  location = glGetUniformLocation(program, "bitmap_texture");
  GL_ASSERT(glUniform1i(location, 0));
}


void GLcontext::vertex_buffers_mesh_create(Mesh *mesh, const size_t max_size)
{
  GLenum target;
  GLint index;

  if (!mesh) 
    return;

  if (glIsVertexArray(mesh->gl_vao)) {
    std::cout << "Error: vertex array already exists for the mesh" << std::endl;
    return;
  }

  GL_ASSERT(glGenVertexArrays(1, &mesh->gl_vao));
  GL_ASSERT(glBindVertexArray(mesh->gl_vao));
  GL_ASSERT(glGenBuffers(8, mesh->gl_vertex_buffers));

  target = GL_ARRAY_BUFFER;
  {
    std::vector<vec3> positions = mesh->positions_get();
    index = 0;
    size_t size = (positions.size()  + max_size) * sizeof(positions[0]);

    GL_ASSERT(glBindBuffer(target, mesh->gl_vertex_buffers[index]));

    if (positions.size() <= 0) {
      GL_ASSERT(glBufferData(target, size, nullptr, GL_STATIC_DRAW));
    } else {
      GL_ASSERT(glBufferData(target, size, positions.data(), GL_STATIC_DRAW));
    }
    GL_ASSERT(glEnableVertexAttribArray(index));
    GL_ASSERT(glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0));
  }

  {
    std::vector<vec3> normals = mesh->normals_get();
    index = 1;
    if (normals.size() > 0) {
      GL_ASSERT(glBindBuffer(target, mesh->gl_vertex_buffers[index]));
      GL_ASSERT(glBufferData(target, normals.size() * sizeof(normals[0]), normals.data(), GL_STATIC_DRAW));
      GL_ASSERT(glEnableVertexAttribArray(index));
      GL_ASSERT(glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0));
    }
  }

  {
    std::vector<vec3> tangents = mesh->tangents_get();
    if (tangents.size() > 0) {
      index = 2;
      GL_ASSERT(glBindBuffer(target, mesh->gl_vertex_buffers[index]));
      GL_ASSERT(glBufferData(target, tangents.size() * sizeof(tangents[0]), tangents.data(), GL_STATIC_DRAW));
      GL_ASSERT(glEnableVertexAttribArray(index));
      GL_ASSERT(glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0));
    }
  }

  {
    std::vector<vec3> bitangents = mesh->bitangents_get();
    if (bitangents.size() > 0) {
      index = 3;
      GL_ASSERT(glBindBuffer(target, mesh->gl_vertex_buffers[index]));
      GL_ASSERT(glBufferData(target, bitangents.size() * sizeof(bitangents[0]), bitangents.data(), GL_STATIC_DRAW));
      GL_ASSERT(glEnableVertexAttribArray(index));
      GL_ASSERT(glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0));
    }
  }

  {
    std::vector<vec3> bone_weights = mesh->bone_weights_get();
    if (bone_weights.size() > 0) {
      index = 4;
      GL_ASSERT(glBindBuffer(target, mesh->gl_vertex_buffers[index]));
      GL_ASSERT(glBufferData(target, bone_weights.size() * sizeof(bone_weights[0]), bone_weights.data(), GL_STATIC_DRAW));
      GL_ASSERT(glEnableVertexAttribArray(index));
      GL_ASSERT(glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0));
    }
  }

  {
    std::vector<ivec3> bone_indices = mesh->bone_indices_get();
    if (bone_indices.size() > 0) {
      index = 5;
      GL_ASSERT(glBindBuffer(target, mesh->gl_vertex_buffers[index]));
      GL_ASSERT(glBufferData(target, bone_indices.size() * sizeof(bone_indices[0]), bone_indices.data(), GL_STATIC_DRAW));
      GL_ASSERT(glEnableVertexAttribArray(index));
      GL_ASSERT(glVertexAttribIPointer(index, 3, GL_INT, 0, 0));
    }
  }

  {
    std::vector<vec2> uvs = mesh->texture_st_get();
    if (uvs.size() > 0) {
      index = 6;
      size_t size = (uvs.size() + max_size) * sizeof(uvs[0]);
      GL_ASSERT(glBindBuffer(target, mesh->gl_vertex_buffers[index]));

      if (uvs.size() <= 0) {
        GL_ASSERT(glBufferData(target, size, nullptr, GL_STATIC_DRAW));
      } else {
        GL_ASSERT(glBufferData(target, size, uvs.data(), GL_STATIC_DRAW));
      }
      GL_ASSERT(glEnableVertexAttribArray(index));
      GL_ASSERT(glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 0, 0));
    }
  }

  {
    target = GL_ELEMENT_ARRAY_BUFFER;
    std::vector<GLshort> indices = mesh->indices_get();
    GL_ASSERT(glBindBuffer(target, mesh->gl_vertex_buffers[7]));
    GL_ASSERT(glBufferData(target, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW));
  }

}


void GLcontext::vertex_buffers_mesh_update(Mesh *mesh)
{
  GLenum target;
  GLint index;

  if (!mesh) return;

  GL_ASSERT(glBindVertexArray(mesh->gl_vao));

  target = GL_ARRAY_BUFFER;
  {
    std::vector<vec3> positions = mesh->positions_get();
    index = 0;
    size_t size = positions.size() * sizeof(positions[0]);
    GL_ASSERT(glBindBuffer(target, mesh->gl_vertex_buffers[index]));
    GL_ASSERT(glBufferSubData(target, 0, size, positions.data()));
  }

  {
    std::vector<vec2> uvs = mesh->texture_st_get();
    if (uvs.size() > 0) {
      index = 6;
      GL_ASSERT(glBindBuffer(target, mesh->gl_vertex_buffers[index]));
      GL_ASSERT(glBufferSubData(target, 0, uvs.size() * sizeof(uvs[0]), uvs.data()));
    }
  }

}


void GLcontext::vertex_buffers_mesh_delete(Mesh *mesh)
{

  if (!mesh) return;

  GL_ASSERT(glDeleteBuffers(8, mesh->gl_vertex_buffers));
  GL_ASSERT(glDeleteVertexArrays(1, &mesh->gl_vao));
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


bool GLcontext::check_version(const int &major)
{
  int maj, min;

  GL_ASSERT(glGetIntegerv(GL_MAJOR_VERSION, &maj));
  GL_ASSERT(glGetIntegerv(GL_MINOR_VERSION, &min));

  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "OpenGL version number: " << maj << "." << min << std::endl;
  std::cout << "GLSL version number: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "GL vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "GL renderer: " << glGetString(GL_RENDERER) << std::endl;


  if (maj < major)
    return false;

  return true;
}


float GLcontext::shadow_jitter_get() 
{
  return ((float) rand() / RAND_MAX) - 0.5f;
}


void GLcontext::shadow_texture_offset_build(int tex_size, int samples_u, int samples_v)
{
  int size = tex_size;
  int samples = samples_u * samples_v;
  int buf_size = size * size * samples * 2;
  float *data = new float[buf_size];

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < samples; k += 2) {
        int x1, y1, x2, y2;

        x1 = k % (samples_u);
        y1 = (samples - 1 - k) / samples_u;
        x2 = (k + 1) % samples_u;
        y2 = (samples - 1 - k - 1) / samples_u;

        vec4 v;
        v.x = (x1 + 0.5f) + shadow_jitter_get();
        v.y = (y1 + 0.5f) + shadow_jitter_get();
        v.z = (x2 + 0.5f) + shadow_jitter_get();
        v.w = (y2 + 0.5f) + shadow_jitter_get();

        v.x /= samples_u;
        v.y /= samples_v;
        v.z /= samples_u;
        v.w /= samples_v;

        int cell = ((k / 2) * size * size + j * size + i) * 4;
        POLL_DEBUG(std::cout, "cell: " << cell);

        data[cell + 0] = sqrtf(v.y) * cosf(2.0f * M_PI * v.x);
        data[cell + 1] = sqrtf(v.y) * sinf(2.0f * M_PI * v.x);
        data[cell + 2] = sqrtf(v.w) * cosf(2.0f * M_PI * v.z);
        data[cell + 3] = sqrtf(v.w) * sinf(2.0f * M_PI * v.z);
      }
    }
  }

  GL_ASSERT(glGenTextures(1, &gl_tex_shadow_sampler));
  GL_ASSERT(glBindTexture(GL_TEXTURE_3D, gl_tex_shadow_sampler));
  GL_ASSERT(glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, size, size, samples / 2, 0, GL_RGBA, GL_FLOAT, data));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));

  delete [] data;
}



mat4 GLcontext::shadow_view_projection_get()
{
  //glm::vec3 lightInvDir = glm::vec3(0.f,1.f,0.f);
  //glm::vec3 lightInvDir = glm::vec3(0, 1, 0);
  glm::vec3 lightInvDir = glm::vec3(0.5f,10,10);
  // Compute the MVP matrix from the light's point of view
  glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
 // glm::mat4 depthProjectionMatrix = glm::perspective(103.f, (float) 720 / (float) 1280, 1.0f, 500.0f);
  glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix;

  return depthMVP;
}


void GLcontext::draw_geometry_all(Scene &scene)
{
  Assets &assets = scene.assets_get();
  Stock_Shaders &shader = assets.stock_shaders_get();


  /* FIXME:
   * Temp shadow code to test concept
   *
   */

  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
  {
    shader.world_geometry.use();
    GL_ASSERT(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl_fb));
    GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    GL_ASSERT(glDrawBuffers(2, draw_bufs));
    GL_ASSERT(glDepthMask(GL_TRUE));
    GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GL_ASSERT(glEnable(GL_DEPTH_TEST));

    GL_ASSERT(glClearColor(0.2f, 0.2f, 0.2f, 1.f));
    for (auto &node: scene.mesh_nodes_get()) {
      draw_node(*node);
    }
  }

  {
    shader.world_geometry_shadow.use();
    mat4 m = shadow_view_projection_get();
    GLint loc = glGetUniformLocation(shader.world_geometry_shadow.program_get(), "shadow_view_projection");
    GL_ASSERT(glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]));

    GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, gl_fb_shadow));
    glViewport(0, 0, shadow_map_width, shadow_map_height);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    GL_ASSERT(glClear(GL_DEPTH_BUFFER_BIT));
    glCullFace(GL_FRONT);
    //glPolygonOffset(100.f, 100.f);

    for (auto &node: scene.mesh_nodes_get()) {
      draw_node(*node);
    }
    glViewport(0, 0, scene.window_get().width_get(), scene.window_get().height_get());
    glCullFace(GL_BACK);
  }

  GL_ASSERT(glDepthMask(GL_FALSE));
  GL_ASSERT(glDisable(GL_DEPTH_TEST));

  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}


void GLcontext::draw_light_all(Scene &scene)
{
  Assets &assets = scene.assets_get();
  Stock_Shaders &shader = assets.stock_shaders_get();
  auto &lights = assets.light_active_get();
  Node *node_screen_quad = assets.stock_nodes_get().screen_quad_get();

  GL_ASSERT(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl_fb));
  GL_ASSERT(glDrawBuffer(GL_COLOR_ATTACHMENT2));

  GL_ASSERT(glEnable(GL_STENCIL_TEST));

  GL_ASSERT(glActiveTexture(GL_TEXTURE0));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, gl_fb_tex_normal));
  GL_ASSERT(glActiveTexture(GL_TEXTURE1));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, gl_fb_tex_diffuse));
  GL_ASSERT(glActiveTexture(GL_TEXTURE2));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, gl_fb_tex_depth));
  GL_ASSERT(glActiveTexture(GL_TEXTURE3));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, gl_fb_tex_shadow));
  GL_ASSERT(glActiveTexture(GL_TEXTURE4));
  GL_ASSERT(glBindTexture(GL_TEXTURE_3D, gl_tex_shadow_sampler));


  for (auto &light: lights) {
    Node *node = light->node_ptr_get();
    if (!node) {
      POLL_ERROR(std::cerr, "Missing node pointer for light: " << light.get());
      continue;
    }

    mat4 &model = node->transform_global_get();
    light->properties_position_set(vec3(model[3][0], model[3][1], model[3][2]));
    uniform_buffers_update_matrices(*node);
    uniform_buffers_update_light(*light);

    Mesh *mesh = node->mesh_get();
    if (!mesh) {
      POLL_ERROR(std::cerr, "No mesh attached to the light: " << node->name_get().c_str());
      continue;
    }

    switch (light->illumination_type_get()) {
      case Light::VOLUME:
        draw_light_volume(*node, shader.world_stencil, shader.world_light);
        break;
      case Light::GLOBAL:
        draw_light_screen(*node_screen_quad, shader.screen_light);
        break;
      default:
        POLL_ERROR(std::cerr, "Illumination type not supported");
        break;
    }

  }
  GL_ASSERT(glDisable(GL_STENCIL_TEST));
}


void GLcontext::draw_light_volume(Node &node, GLshader &shader_stencil, GLshader &shader_light)
{
  NODE_VALIDATE(node);

  shader_stencil.use();
  GL_ASSERT(glDrawBuffer(GL_NONE));
  GL_ASSERT(glClear(GL_STENCIL_BUFFER_BIT));
  GL_ASSERT(glEnable(GL_DEPTH_TEST));
  GL_ASSERT(glDisable(GL_CULL_FACE));
  GL_ASSERT(glStencilFunc(GL_ALWAYS, 0, 0));
  GL_ASSERT(glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP));
  GL_ASSERT(glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP));
  draw_mesh(node);

  shader_light.use();
  GL_ASSERT(glDrawBuffer(GL_COLOR_ATTACHMENT2));
  GL_ASSERT(glStencilFunc(GL_NOTEQUAL, 0, 0xFF));
  GL_ASSERT(glDisable(GL_DEPTH_TEST));
  GL_ASSERT(glEnable(GL_BLEND));
  GL_ASSERT(glBlendEquation(GL_FUNC_ADD));
  GL_ASSERT(glBlendFunc(GL_ONE, GL_ONE));
  GL_ASSERT(glEnable(GL_CULL_FACE));
  GL_ASSERT(glCullFace(GL_FRONT));
  draw_mesh(node);

  GL_ASSERT(glCullFace(GL_BACK));
  GL_ASSERT(glDisable(GL_BLEND));
}


void GLcontext::draw_light_screen(Node &node, GLshader &shader_quad_light)
{
  NODE_VALIDATE(node);

  shader_quad_light.use();
  /* FIXME: shadow stuff, temp 
   *
   */

  mat4 m = shadow_view_projection_get();
  GLint loc = glGetUniformLocation(shader_quad_light.program_get(), "shadow_view_projection");
  GL_ASSERT(glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]));

  GL_ASSERT(glClear(GL_STENCIL_BUFFER_BIT));
  GL_ASSERT(glDisable(GL_STENCIL_TEST));
  GL_ASSERT(glEnable(GL_BLEND));
  GL_ASSERT(glBlendEquation(GL_FUNC_ADD));
  GL_ASSERT(glBlendFunc(GL_ONE, GL_ONE));
  draw_mesh(node);

  GL_ASSERT(glDisable(GL_BLEND));
  GL_ASSERT(glEnable(GL_STENCIL_TEST));
}


void GLcontext::draw_node(Node &node)
{
  NODE_VALIDATE(node);
  Mesh *mesh = node.mesh_get();

  if (!mesh) {
    POLL_ERROR(std::cerr, "No mesh attached to node: " <<  node.name_get().c_str());
    return;
  }

  uniform_buffers_update_state(node);
  uniform_buffers_update_matrices(node);

  {
    Armature *armature = node.armature_get();
    if (armature) uniform_buffers_update_armature(*armature);
  }

  { 
    Material *material = node.material_get();
    if (material) uniform_buffers_update_material(*material);
  }

  if (node.state_get().cubemap_skybox) glDepthMask(GL_FALSE);
  draw_mesh(node);
  if (node.state_get().cubemap_skybox) glDepthMask(GL_TRUE);

}


void GLcontext::framebuffer_check_status() 
{
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (GL_FRAMEBUFFER_COMPLETE != status) {
    fprintf (stderr, "ERROR: incomplete framebuffer\n");
    if (GL_FRAMEBUFFER_UNDEFINED == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_UNDEFINED\n");
    } else if (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
    } else if (GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
    } else if (GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n");
    } else if (GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER== status) {
      fprintf (stderr, "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n");
    } else if (GL_FRAMEBUFFER_UNSUPPORTED == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_UNSUPPORTED\n");
    } else if (GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n");
    } else if (GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS == status) {
      fprintf (stderr, "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS\n");
    } else {
      fprintf (stderr, "unspecified error\n");
    }
    exit(-1);
  }
}


void GLcontext::framebuffer_delete()
{

}


void GLcontext::framebuffer_create()
{
  auto width = window_ptr->width_get();
  auto height = window_ptr->height_get();

  GL_ASSERT(glGenTextures(1, &gl_fb_tex_normal));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, gl_fb_tex_normal));
  GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

  GL_ASSERT(glGenTextures(1, &gl_fb_tex_diffuse));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, gl_fb_tex_diffuse));
  GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

  GL_ASSERT(glGenFramebuffers (1, &gl_fb));
  GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, gl_fb));
  GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_fb_tex_normal, 0));
  GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gl_fb_tex_diffuse, 0));
  //GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  //GL_ASSERT(glDrawBuffers(2, draw_bufs));

  framebuffer_check_status();


  GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };
  GL_ASSERT(glGenTextures(1, &gl_fb_tex_depth));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, gl_fb_tex_depth));
  GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, gl_fb_tex_depth, 0));

  GL_ASSERT(glGenTextures(1, &gl_fb_tex_final));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, gl_fb_tex_final));
  GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
  // GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  // GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gl_fb_tex_final, 0));

  GL_ASSERT(glGenFramebuffers (1, &gl_fb_shadow));
  GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, gl_fb_shadow));

  GL_ASSERT(glGenTextures(1, &gl_fb_tex_shadow));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, gl_fb_tex_shadow));
  GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadow_map_width, shadow_map_height,
        0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL));
  /*
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  */
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
  GL_ASSERT(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS));
  GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gl_fb_tex_shadow, 0));

  GL_ASSERT(glDrawBuffer(GL_NONE));
  GL_ASSERT(glReadBuffer(GL_NONE));
}



void GLcontext::texture_create(Texture &texture, GLenum active_texture, GLint filter,
    GLint wrap, GLint internal_format, GLenum format,  
    const bool unpack_align) 
{
  if (glIsTexture(texture.gl_texture)) {
    return;
  }

  GL_ASSERT(glGenTextures(1, &texture.gl_texture));
  GL_ASSERT(glActiveTexture(active_texture));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, texture.gl_texture));

  if (unpack_align)
    GL_ASSERT(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));

  if (wrap != GL_NONE) { 
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap));
  }

  GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, texture.image->width, texture.image->height,
        0, format, GL_UNSIGNED_BYTE, &texture.image->data_get()));
}


void GLcontext::texture_cubemap_create(Cubemap_Item &item) 
{
  // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  GL_ASSERT(glTexImage2D(item.target, 0, GL_RGB, item.texture.image->width, item.texture.image->height,
        0, GL_RGB, GL_UNSIGNED_BYTE, &item.texture.image->data_get()));

  GL_ASSERT(glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  GL_ASSERT(glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  GL_ASSERT(glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
  GL_ASSERT(glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GL_ASSERT(glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
}


void GLcontext::texture_cubemap_delete(Cubemap &cubemap)
{
  if (!glIsTexture(cubemap.gl_texture)) return;
  GL_ASSERT(glDeleteTextures(1, &cubemap.gl_texture));
}


