#include "glcontext.h"


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
  glm::vec4 color(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glClearColor(color.x, color.y, color.z, color.a);
}


void GLcontext::draw(Node &node)
{
  Mesh *mesh = node.mesh;
  if (!mesh) return;

  glBindVertexArray(node.gl_vao);
  GLsizei count = (GLsizei) mesh->num_indices_get();
  uniform_buffers_update_node(node);
  uniform_buffers_update_mesh(*mesh);
  glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
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
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glViewport(0, 0, width, height);

  return true;
}


void GLcontext::polygon_mesh_toggle(bool tog)
{
  glPolygonMode(GL_FRONT_AND_BACK, tog ? GL_FILL : GL_LINE);
}


void GLcontext::texture_create(Texture &texture, GLenum n) 
{
  glGenTextures(1, &texture.gl_texture);
  glActiveTexture(n);
  glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.image->width, texture.image->height,
      0, GL_RGB, GL_UNSIGNED_BYTE, texture.image->data);
}


void GLcontext::uniform_buffers_init(GLshader &shader)
{
  GLuint program = shader.program;
  GLint uniform_block_index;
  GLenum target;
  GLuint bindPointIndex;

  uniform_block_index = shader.get_block_index("GlobalMatrices");
  glUniformBlockBinding(program, uniform_block_index, UB_GLOBALMATRICES);
  uniform_block_index = shader.get_block_index("Matrices");
  glUniformBlockBinding(program, uniform_block_index, UB_MATRICES);
  uniform_block_index = shader.get_block_index("Armature");
  glUniformBlockBinding(program, uniform_block_index, UB_ARMATURE);
  uniform_block_index = shader.get_block_index("Debug");
  glUniformBlockBinding(program, uniform_block_index, UB_DEBUG);

  target = GL_UNIFORM_BUFFER;
  {
    glm::mat4 matrix[2];
    bindPointIndex = UB_GLOBALMATRICES;
    glGenBuffers(1, &gl_buffer_globalmatrices);
    glBindBuffer(target, gl_buffer_globalmatrices);
    glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW);
    glBindBufferRange(target, bindPointIndex, gl_buffer_globalmatrices, 0, sizeof(matrix));
    glBindBufferBase(target, bindPointIndex, gl_buffer_globalmatrices);
  }

  {
    glm::mat4 matrix;
    bindPointIndex = UB_MATRICES;
    glGenBuffers(1, &gl_buffer_matrices);
    glBindBuffer(target, gl_buffer_matrices);
    glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW);
    glBindBufferRange(target, bindPointIndex, gl_buffer_matrices, 0, sizeof(matrix));
    glBindBufferBase(target, bindPointIndex, gl_buffer_matrices);
  }

  {
    glm::mat4 matrix[64];
    bindPointIndex = UB_ARMATURE;
    glGenBuffers(1, &gl_buffer_armature);
    glBindBuffer(target, gl_buffer_armature);
    glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW);
    glBindBufferRange(target, bindPointIndex, gl_buffer_armature, 0, sizeof(matrix));
    glBindBufferBase(target, bindPointIndex, gl_buffer_armature);
  }

  {
    glm::vec4 debug(0.5, 0.5, 0.5, 0);
    bindPointIndex = UB_DEBUG;
    glGenBuffers(1, &gl_buffer_debug);
    glBindBuffer(target, gl_buffer_debug);
    glBufferData(target, sizeof(debug), &debug, GL_STREAM_DRAW);
    glBindBufferRange(target, bindPointIndex, gl_buffer_debug, 0, sizeof(debug));
    glBindBufferBase(target, bindPointIndex, gl_buffer_debug);
  }

  {
    GLint location;
    location = glGetUniformLocation(program, "ColorTex");
    std::cout << "Colortex on location: " << location << std::endl;
    glUniform1i(location, 0);

    location = glGetUniformLocation(program, "NormalMapTex");
    glUniform1i(location, 1);
    std::cout << "NormalMapTex on location: " << location << std::endl;

    location = glGetUniformLocation(program, "SpecularTex");
    glUniform1i(location, 2);
    std::cout << "Specular on location: " << location << std::endl;
  }

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
}


void GLcontext::uniform_buffers_update_debug(glm::vec4 &data)
{
  GLenum target = GL_UNIFORM_BUFFER;
  GLintptr offset = 0;
  glBindBuffer(target, gl_buffer_debug);
  glBufferSubData(target, offset, sizeof(data), &data);
}


void GLcontext::uniform_buffers_update_mesh(Mesh &mesh)
{
  glm::mat4 m;
  m = mesh.model;

  GLenum target = GL_UNIFORM_BUFFER;
  GLintptr offset = 0;
  glBindBuffer(target, gl_buffer_matrices);
  glBufferSubData(target, offset, sizeof(m), &m);
}

void GLcontext::uniform_buffers_update_node(Node &node)
{
  GLenum target = GL_UNIFORM_BUFFER;
  GLintptr offset = 0;
  Armature *armature = node.armature;
  Material *material = node.material;

  if (armature) {
    glBindBuffer(target, gl_buffer_armature);
    glBufferSubData(target, offset, armature->skinning_matrices.size() * sizeof(armature->skinning_matrices[0]), 
        armature->skinning_matrices.data());
  }

  if (material) {
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
  }
}


void GLcontext::vertex_buffers_add(Node &node)
{
  GLuint buffer;
  GLenum target;
  GLint index;
  Mesh *mesh = node.mesh;
  Material *material = node.material;

  if  (mesh) {
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec4> tangents;
    std::vector<glm::vec4> bitangents;
    std::vector<glm::vec4> weights;
    std::vector<glm::ivec4> bone_indices;
    std::vector<glm::vec2> uvs;
    std::vector<GLshort> indices;
    mesh->buffer_data_get(&vertices, &normals, &tangents, &bitangents,
        &weights, &bone_indices, &uvs, &indices);

    glGenVertexArrays(1, &node.gl_vao);
    glBindVertexArray(node.gl_vao);

    target = GL_ARRAY_BUFFER;
    index = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);

    index = 1;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, normals.size() * sizeof(normals[0]), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);

    index = 2;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, tangents.size() * sizeof(tangents[0]), tangents.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);

    index = 3;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, bitangents.size() * sizeof(bitangents[0]), bitangents.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);

    index = 4;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, weights.size() * sizeof(weights[0]), weights.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);

    index = 5;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, bone_indices.size() * sizeof(bone_indices[0]), bone_indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribIPointer(index, 4, GL_INT, 0, 0);

    index = 6;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, uvs.size() * sizeof(uvs[0]), uvs.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 0, 0);

    target = GL_ELEMENT_ARRAY_BUFFER;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
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
