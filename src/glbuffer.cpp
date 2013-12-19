#include "glbuffer.h"

using std::move;
using std::cout;
using std::endl;
using std::vector;
using std::unique_ptr;
using std::move;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ivec4;

//
// GLattribute
//
GLattribute::GLattribute(const GLuint & index, const GLint & size,
                         const GLenum type):
  index(index), 
  size(size), 
  type(type)
{
}
  
  
GLattribute::~GLattribute() 
{
}


void GLattribute::enable()
{
  glEnableVertexAttribArray(index);

  switch (type) {
    case GL_FLOAT:
      glVertexAttribPointer(index, size, type, GL_FALSE, 0, 0);
      break;
    case GL_INT:
      glVertexAttribIPointer(index, size, type, 0, 0);
      break;
    default:
      cout << "Fragmic warning: attribute type not supported" << endl;
      break;
  }

}


void GLattribute::disable()
{
  glDisableVertexAttribArray(index);
}

//
// GLbuffer
//
void GLbuffer::init()
{
  generate();
  bind();
}


void GLbuffer::createAttribute(const GLuint & index, const GLint & size,
                               const GLenum type)
{
  if (target == GL_UNIFORM_BUFFER) {
    return;
  }

  std::unique_ptr<GLattribute> attributePtr(new GLattribute(index, size, type));
  attribute = move(attributePtr);
}


void GLbuffer::enableAttribute()
{
  if (!attribute) {
    cout << "Fragmic warning: no attribute object to enable" << endl;
    return;
  }

  attribute->enable();
}


void GLbuffer::disableAttribute()
{
  if (!attribute) {
    cout << "Fragmic warning: no attribute object to disable" << endl;
    return;
  }

  attribute->disable();
}


void GLbuffer::term()
{
  glDeleteBuffers(1, &buffer);
}


void GLbuffer::bind()
{
  glBindBuffer(target, buffer);
}


void GLbuffer::unbind()
{
  glBindBuffer(target, 0);
}


void GLbuffer::bindBase()
{
  glBindBufferBase(target, bindPointIndex, buffer);
}


void GLbuffer::bindBufferRange(GLsizeiptr size)
{
  glBindBufferRange(target, bindPointIndex, buffer, 0, size);
}


void GLbuffer::generate()
{
  glGenBuffers(1, &buffer);

  if (glIsBuffer(buffer) != GL_FALSE) {
    std::cout << "Fragmic error: could not create buffer" << std::endl;
  }
}


void GLbuffer::create(const vector < vec2 > &data, const int &attributeIndex)
{
  copy(data);
  createAttribute(attributeIndex, 2, GL_FLOAT);
}


void GLbuffer::create(const vector < vec3 > &data, const int &attributeIndex)
{
  copy(data);
  createAttribute(attributeIndex, 3, GL_FLOAT);
}


void GLbuffer::create(const vector < vec4 > &data, const int &attributeIndex)
{
  copy(data);
  createAttribute(attributeIndex, 4, GL_FLOAT);
}


void GLbuffer::create(const vector < ivec4 > &data, const int &attributeIndex)
{
  copy(data);
  createAttribute(attributeIndex, 4, GL_INT);
}


void GLbuffer::create(const vector <GLshort> &data)
{
  copy(data);
}


//
// GLvertexArray
//
GLvertexArray::GLvertexArray():
  vao(false) 
{
}


GLvertexArray::~GLvertexArray() 
{
}


void GLvertexArray::generate()
{
  glGenVertexArrays(1, &vao);
}


void GLvertexArray::term()
{
  glDeleteVertexArrays(1, &vao);
}


void GLvertexArray::bind()
{
  if (!vao) {
    std::cout << "Fragmic warning: vertex array object not set" << std::endl;
    return;
  }

  glBindVertexArray(vao);
}


void GLvertexArray::unbind()
{
  glBindVertexArray(0);
}


void GLvertexArray::create(const std::vector<std::unique_ptr<GLindexBuffer>> &indexBuffers,
                           const std::vector<std::unique_ptr<GLvertexBuffer>> &vertexBuffers)
{
  generate();
  bind();
  for (auto &buffer_entry : indexBuffers) {
    auto & buffer = *buffer_entry;
    buffer.bind();
  }

  for (auto &buffer_entry : vertexBuffers) {
    auto &buffer = *buffer_entry;
    buffer.bind();
    buffer.enableAttribute();
  }
  unbind();
}
