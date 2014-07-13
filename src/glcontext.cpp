#include "glcontext.h"
#include "material.h"
#include "node.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


GLcontext::GLcontext()
{
}


GLcontext::~GLcontext()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void GLcontext::check_error()
{
  try {
    GLenum gl_error = glGetError();
    if (GL_NO_ERROR != gl_error) {
      std::cout << std::string("OpenGL error: ") << reinterpret_cast<const char *>(gluErrorString(gl_error)) << std::endl;   
    }
  } catch(std::exception & e) {
    std::cerr << e.what() << std::endl;
  }

}


void GLcontext::clear()
{
  glm::vec4 color(0.5, 0.5, 0.5, 1.0);
  //  glm::vec4 color(0, 0, 0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glClearColor(color.x, color.y, color.z, color.a);
}


void GLcontext::node_draw(Node &node)
{
  Mesh *mesh = node.mesh;
  if (!mesh) return;

  uniform_buffers_update_node(node);

  if (node.state.cubemap_skybox) glDepthMask(GL_FALSE);

  glBindVertexArray(node.gl_vao);
  GLsizei count = (GLsizei) mesh->num_indices_get();
  if (count <= 0) {
    glDrawArrays(mesh->mode, 0, mesh->num_vertices_get());
  } else {
    glDrawElements(mesh->mode, count, GL_UNSIGNED_SHORT, 0);
  }

  if (node.state.cubemap_skybox) glDepthMask(GL_TRUE);
}

void GLcontext::framebuffer_draw_texture(Scene &scene)
{
  for (auto &node: scene.render_list_get()) {
    node_draw(*node);
  }

}


void GLcontext::framebuffer_draw_screen()
{

}


void GLcontext::framebuffer_node_set(Node &node)
{
  fb_node = &node;
}


bool GLcontext::init(const int width, const int height)
{
  glewExperimental= GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cout << "GLcontext ERROR: failed to initalize GLEW" << std::endl;
    return false;
  }
  check_error();
  check_version(3);

  glEnable(GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
  glEnable(GL_CULL_FACE); // cull face
  glCullFace(GL_BACK); // cull back face
  glFrontFace(GL_CCW); // set counter-clock-wise vertex order to mean the front
  glViewport(0, 0, width, height);

  framebuffer_create(width, height);

  return true;
}


void GLcontext::polygon_mesh_toggle(bool tog)
{
  glPolygonMode(GL_FRONT_AND_BACK, tog ? GL_FILL : GL_LINE);
}


void GLcontext::uniform_buffers_create(GLshader &shader)
{
  GLuint program = shader.program;
  GLenum target = GL_UNIFORM_BUFFER;
  GLuint block_index;
  GLuint bind_index;

  {
    glm::mat4 matrix[2];
    bind_index = UB_GLOBALMATRICES;
    block_index = shader.get_block_index("GlobalMatrices");
    glGenBuffers(1, &gl_buffer_globalmatrices);
    glBindBuffer(target, gl_buffer_globalmatrices);
    glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW);
    glUniformBlockBinding(program, block_index, bind_index);
    glBindBufferBase(target, bind_index, gl_buffer_globalmatrices);
    //glBindBufferRange(target, bind_index, gl_buffer_globalmatrices, 0, sizeof(matrix));
    glBindBuffer(target, 0);
  }

  {
    glm::mat4 matrix;
    bind_index = UB_MATRICES;
    block_index = shader.get_block_index("Matrices");
    glGenBuffers(1, &gl_buffer_matrices);
    glBindBuffer(target, gl_buffer_matrices);
    glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW);
    glUniformBlockBinding(program, block_index, bind_index);
    glBindBufferBase(target, bind_index, gl_buffer_matrices);
    //glBindBufferRange(target, bind_index, gl_buffer_matrices, 0, sizeof(matrix));
    glBindBuffer(target, 0);
  }

  {
    glm::mat4 matrix[64];
    bind_index = UB_ARMATURE;
    block_index = shader.get_block_index("Armature");
    glGenBuffers(1, &gl_buffer_armature);
    glBindBuffer(target, gl_buffer_armature);
    glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW);
    glUniformBlockBinding(program, block_index, bind_index);
    glBindBufferBase(target, bind_index, gl_buffer_armature);
    //glBindBufferRange(target, bind_index, gl_buffer_armature, 0, sizeof(matrix));
    glBindBuffer(target, 0);
  }

  {
    Material_Properties properties;
    properties.Ka = glm::vec4(1.f, 0.f, 0.f, 1.f);
    properties.Kd = glm::vec4(0.f, 1.f, 0.f, 1.f);
    properties.Ks = glm::vec4(0.f, 0.f, 1.f, 1.f);
    properties.shininess = 0.f;

    bind_index = UB_MATERIAL;
    block_index = shader.get_block_index("Material");
    glGenBuffers(1, &gl_buffer_material);
    glBindBuffer(target, gl_buffer_material);
    glBufferData(target, sizeof(properties), &properties, GL_STREAM_DRAW);
    glUniformBlockBinding(program, block_index, bind_index);
    glBindBufferBase(target, bind_index, gl_buffer_material);
    //glBindBufferRange(target, bind_index, gl_buffer_material, 0, sizeof(properties));
    glBindBuffer(target, 0);

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
    block_index = shader.get_block_index("State");
    glGenBuffers(1, &gl_buffer_state);
    glBindBuffer(target, gl_buffer_state);
    glBufferData(target, sizeof(state), &state, GL_STREAM_DRAW);
    glUniformBlockBinding(program, block_index, bind_index);
    glBindBufferBase(target, bind_index, gl_buffer_state);
    //glBindBufferRange(target, bind_index, gl_buffer_state, 0, sizeof(state));
    glBindBuffer(target, 0);
  }


  {
    GLint location;
    location = glGetUniformLocation(program, "diffuse_texture");
    glUniform1i(location, 0);
    location = glGetUniformLocation(program, "normal_texture");
    glUniform1i(location, 1);
    location = glGetUniformLocation(program, "specular_texture");
    glUniform1i(location, 2);
    location = glGetUniformLocation(program, "cube_texture");
    glUniform1i(location, 3);
  }

  gl_uniform_camera_pos = glGetUniformLocation(program, "camera_position_world");
}


void GLcontext::uniform_buffers_delete()
{
  glDeleteBuffers(1, &gl_buffer_globalmatrices);
  glDeleteBuffers(1, &gl_buffer_matrices);
  glDeleteBuffers(1, &gl_buffer_armature);
  glDeleteBuffers(1, &gl_buffer_material);
  glDeleteBuffers(1, &gl_buffer_state);
}


void GLcontext::uniform_buffers_update_camera(Camera &camera)
{
  GLenum target = GL_UNIFORM_BUFFER;
  GLintptr offset = 0;
  glm::mat4 data[2];
  data[0] = camera.perspective;
  data[1] = camera.view;
  glBindBuffer(target, gl_buffer_globalmatrices);
  glBufferSubData(target, offset, sizeof(data), &data);
  glBindBuffer(target, 0);

  glUniform3f(gl_uniform_camera_pos, camera.position.x, camera.position.y, camera.position.z);
}


void GLcontext::uniform_buffers_update_material(Material &material)
{
  GLenum target = GL_UNIFORM_BUFFER;
  GLintptr offset = 0;
  glBindBuffer(target, gl_buffer_material);
  glBufferSubData(target, offset, sizeof(material.material_block), &material.material_block);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

}


void GLcontext::uniform_buffers_update_mesh(Mesh &mesh)
{
  glm::mat4 m;
  m = mesh.model;

  GLenum target = GL_UNIFORM_BUFFER;
  GLintptr offset = 0;
  glBindBuffer(target, gl_buffer_matrices);
  glBufferSubData(target, offset, sizeof(m), &m);
  glBindBuffer(target, 0);
}


void GLcontext::uniform_buffers_update_node(Node &node)
{
  GLintptr offset = 0;
  Armature *armature = node.armature;
  Material *material = node.material;
  Mesh *mesh = node.mesh;

  uniform_buffers_update_state(node);
  uniform_buffers_update_mesh(*mesh);

  if (armature) {
    GLenum target = GL_UNIFORM_BUFFER;
    glBindBuffer(target, gl_buffer_armature);
    glBufferSubData(target, offset, armature->skinning_matrices.size() * sizeof(armature->skinning_matrices[0]), 
        armature->skinning_matrices.data());
    glBindBuffer(target, 0);
  }


  if (material) {
    uniform_buffers_update_material(*material);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (material->diffuse) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, material->diffuse->gl_texture);
    }
    if (material->normal) {
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, material->normal->gl_texture);
    }
    if (material->specular) {
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, material->specular->gl_texture);
    }
    if (material->cubemap) {
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_CUBE_MAP, material->cubemap->gl_texture);
    }
  }
}


void GLcontext::uniform_buffers_update_state(Node &node)
{
  GLenum target = GL_UNIFORM_BUFFER;
  GLintptr offset = 0;
  glBindBuffer(target, gl_buffer_state);
  glBufferSubData(target, offset, sizeof(node.state), &node.state);
  glBindBuffer(target, 0);
}



void GLcontext::vertex_buffers_create(Node &node)
{
  GLenum target;
  GLint index;
  Mesh *mesh = node.mesh;
  Material *material = node.material;

  if (!mesh) return;

  glGenVertexArrays(1, &node.gl_vao);
  glBindVertexArray(node.gl_vao);
  glGenBuffers(8, gl_vertex_buffers);

  target = GL_ARRAY_BUFFER;
  {
    std::vector<glm::vec3> positions = mesh->positions_get();
    index = 0;
    glBindBuffer(target, gl_vertex_buffers[index]);
    glBufferData(target, positions.size() * sizeof(positions[0]), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
  }

  {
    std::vector<glm::vec3> normals = mesh->normals_get();
    index = 1;
    if (normals.size() > 0) {
      glBindBuffer(target, gl_vertex_buffers[index]);
      glBufferData(target, normals.size() * sizeof(normals[0]), normals.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(index);
      glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
  }

  {
    std::vector<glm::vec3> tangents = mesh->tangents_get();
    if (tangents.size() > 0) {
      index = 2;
      glBindBuffer(target, gl_vertex_buffers[index]);
      glBufferData(target, tangents.size() * sizeof(tangents[0]), tangents.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(index);
      glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
  }

  {
    std::vector<glm::vec3> bitangents = mesh->bitangents_get();
    if (bitangents.size() > 0) {
      index = 3;
      glBindBuffer(target, gl_vertex_buffers[index]);
      glBufferData(target, bitangents.size() * sizeof(bitangents[0]), bitangents.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(index);
      glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
  }

  {
    std::vector<glm::vec3> bone_weights = mesh->bone_weights_get();
    if (bone_weights.size() > 0) {
      index = 4;
      glBindBuffer(target, gl_vertex_buffers[index]);
      glBufferData(target, bone_weights.size() * sizeof(bone_weights[0]), bone_weights.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(index);
      glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
  }

  {
    std::vector<glm::ivec3> bone_indices = mesh->bone_indices_get();
    if (bone_indices.size() > 0) {
      index = 5;
      glBindBuffer(target, gl_vertex_buffers[index]);
      glBufferData(target, bone_indices.size() * sizeof(bone_indices[0]), bone_indices.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(index);
      glVertexAttribIPointer(index, 3, GL_INT, 0, 0);
    }
  }

  {
    std::vector<glm::vec2> uvs = mesh->texture_st_get();
    if (uvs.size() > 0) {
      index = 6;
      glBindBuffer(target, gl_vertex_buffers[index]);
      glBufferData(target, uvs.size() * sizeof(uvs[0]), uvs.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(index);
      glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }
  }

  {
    target = GL_ELEMENT_ARRAY_BUFFER;
    std::vector<GLshort> indices = mesh->indices_get();
    glBindBuffer(target, gl_vertex_buffers[7]);
    glBufferData(target, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
  }

  if (material) {
    if (material->diffuse) {
      texture_create(*material->diffuse, GL_TEXTURE0);
    }
    if (material->normal) {
      texture_create(*material->normal, GL_TEXTURE1);
    }
    if (material->specular) {
      texture_create(*material->specular, GL_TEXTURE2);
    }
    if (material->cubemap) {
      if (!glIsTexture(material->cubemap->gl_texture)) {
        glGenTextures(1,  &material->cubemap->gl_texture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, material->cubemap->gl_texture);
        texture_cubemap_create(material->cubemap->front);
        texture_cubemap_create(material->cubemap->back);
        texture_cubemap_create(material->cubemap->top);
        texture_cubemap_create(material->cubemap->bottom);
        texture_cubemap_create(material->cubemap->left);
        texture_cubemap_create(material->cubemap->right);
      }
    }
  }
}


void GLcontext::vertex_buffers_delete(Node &node)
{
  Mesh *mesh = node.mesh;
  Material *material = node.material;

  if (mesh) {
    glDeleteBuffers(8, gl_vertex_buffers);
    glDeleteVertexArrays(1, &node.gl_vao);
  }
  if (material) {
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

}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


bool GLcontext::check_version(const int &major)
{
  int maj, min;

  glGetIntegerv(GL_MAJOR_VERSION, &maj);
  glGetIntegerv(GL_MINOR_VERSION, &min);

  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "OpenGL version number: " << maj << "." << min << std::endl;
  std::cout << "GLSL version number: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "GL vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "GL renderer: " << glGetString(GL_RENDERER) << std::endl;


  if (maj < major)
    return false;

  return true;
}


void GLcontext::framebuffer_create(const int width, const int height)
{
  glGenFramebuffers (1, &fb);

  /* create the texture that will be attached to the fb. should be the same
     dimensions as the viewport */
  glGenTextures (1, &fb_tex);
  glBindTexture (GL_TEXTURE_2D, fb_tex);
  glTexImage2D (
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      width,
      height,
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      NULL
      );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  /* attach the texture to the framebuffer */
  glBindFramebuffer (GL_FRAMEBUFFER, fb);
  glFramebufferTexture2D (
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_tex, 0
      );
  /* create a renderbuffer which allows depth-testing in the framebuffer */
  GLuint rb = 0;
  glGenRenderbuffers (1, &rb);
  glBindRenderbuffer (GL_RENDERBUFFER, rb);
  glRenderbufferStorage (
      GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height
      );
  /* attach renderbuffer to framebuffer */
  glFramebufferRenderbuffer (
      GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb
      );
  /* tell the framebuffer to expect a colour output attachment (our texture) */
  GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers (1, draw_bufs);

  /* validate the framebuffer - an 'incomplete' error tells us if an invalid
     image format is attached or if the glDrawBuffers information is invalid */
  GLenum status = glCheckFramebufferStatus (GL_FRAMEBUFFER);
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

  /* re-bind the default framebuffer as a safe precaution */
  glBindFramebuffer (GL_FRAMEBUFFER, 0);
}


void GLcontext::framebuffer_delete()
{

}


void GLcontext::texture_create(Texture &texture, GLenum n) 
{
  if (glIsTexture(texture.gl_texture)) {
    return;
  }

  glGenTextures(1, &texture.gl_texture);
  glActiveTexture(n);
  glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.image->width, texture.image->height,
      0, GL_RGB, GL_UNSIGNED_BYTE, texture.image->data);
}


void GLcontext::texture_cubemap_create(Cubemap_Item &item) 
{
  // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(item.target, 0, GL_RGB, item.texture.image->width, item.texture.image->height,
      0, GL_RGB, GL_UNSIGNED_BYTE, item.texture.image->data);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


void GLcontext::texture_cubemap_delete(Cubemap &cubemap)
{
  if (glIsTexture(cubemap.gl_texture)) {
    std::cout << "Deleting opengl cubemap texture buffer" << std::endl;
    glDeleteTextures(1, &cubemap.gl_texture);
  }
}


void GLcontext::texture_delete(Texture &texture)
{
  if (glIsTexture(texture.gl_texture)) {
    std::cout << "Deleting opengl texture buffer: " << texture.filename << "'" <<  std::endl;
    glDeleteTextures(1, &texture.gl_texture);
  } 
}


