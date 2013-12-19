#ifndef GLBUFFER_H
#define GLBUFFER_H

#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <memory>

#include "gldefaults.h"

class GLattribute {
  GLuint index;
  GLint size;
  GLenum type;

  public:
  GLattribute(const GLuint &index, const GLint &size, const GLenum type);
  ~GLattribute();


  void enable();
  void disable();
};

class GLbuffer {

  private:
    GLenum target;
    GLuint buffer;
    std::unique_ptr<GLattribute> attribute;
    GLuint bindPointIndex;

    template <class T> 
      void copy(const std::vector<T> &data) 
      {
        init();
        glBufferData(target, data.size() * sizeof(data[0]),
            data.data(), GL_STATIC_DRAW);
      } 

    template <typename T> 
      void copy(const T &data) {
        init();
        glBufferData(target, sizeof(data), &data, GL_STREAM_DRAW);
      }

    void bindBase();
    void bindBufferRange(GLsizeiptr size);
    void createAttribute(const GLuint & index, const GLint & size,
        const GLenum type);
    void init();
    void generate();
    void term();
    void unbind();

  public:

    GLbuffer(const GLenum & target):
      target(target)
      {
      };

    ~GLbuffer() {
    };

    void bind();
    void enableAttribute();
    void disableAttribute();

    //
    // CREATE METHODS
    //
    void create(const std::vector < glm::vec2 > &data, const int &attributeIndex);
    void create(const std::vector < glm::vec3 > &data, const int &attributeIndex);
    void create(const std::vector < glm::vec4 > &data, const int &attributeIndex);
    void create(const std::vector < glm::ivec4 > &data,
        const int &attributeIndex);
    void create(const std::vector < GLshort > &data);

    template <typename T> 
      void create(const T & data, const int &index)
      {
        std::cout << "Creting uniform buffer" << std::endl;

        bindPointIndex = index;
        copy(data);

        if (target == GL_UNIFORM_BUFFER) {
          bindBufferRange(sizeof(data));
          bindBase();
        }

        unbind();
      }

    //
    // UPDATE METHODS
    //
    template <typename T> 
      void update(const T & data, GLintptr offset) {
        bind();
        glBufferSubData(target, offset, sizeof(data), &data);
        unbind();
      }

    template < typename T >
      void update(const std::vector <T> &data, GLintptr offset) {
        bind();
        glBufferSubData(target, offset, data.size() * sizeof(data[0]), data.data());
        unbind();
      }



};


class GLvertexBuffer:public GLbuffer {
  public:
    GLvertexBuffer():
      GLbuffer(GL_ARRAY_BUFFER) 
    {
    }

    
    ~GLvertexBuffer() 
    {
    }

};


class GLindexBuffer: public GLbuffer {
  public:
    GLindexBuffer():
      GLbuffer(GL_ELEMENT_ARRAY_BUFFER) 
    {
    } 
    

    ~GLindexBuffer() 
    {
    }

};


class GLuniformBuffer: public GLbuffer {
  private:
    GLint uboIndex;
    GLint baseIndeX;
    std::string blockName;

  public:
    GLuniformBuffer(const std::string &blockName): 
      GLbuffer(GL_UNIFORM_BUFFER), 
      blockName(blockName) 
    {
    }


    ~GLuniformBuffer() 
    {
    }

};


class GLvertexArray {
  private:
    GLuint vao;

    void generate();
    void term();

  public:
    GLvertexArray();
    ~GLvertexArray();

    void bind();
    void unbind();
    void create(const std::vector<std::unique_ptr<GLindexBuffer>> &indexBuffers,
                const std::vector<std::unique_ptr<GLvertexBuffer>> &vertexBuffers);
};




#endif
