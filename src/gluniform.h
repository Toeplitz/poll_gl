#ifndef GLUNIFORM_H
#define GLUNIFORM_H

#include <stdexcept>
#include <GL/glew.h>
#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "camera.h"
#include "glshader.h"
#include "glbuffer.h"
#include "gldefaults.h"


class GLuniform {
  private:
    typedef std::map<std::string, std::unique_ptr<GLuniformBuffer>> UniformBufferMap;
    UniformBufferMap uniformBufferMap;

  public:
    GLuniform() {}
    ~GLuniform() {}

    void updateBlockIndexMap(const std::vector<std::unique_ptr<GLshader>> shaders) 
    {
      for (auto &shader_entry : shaders) {
        GLshader &shader = *shader_entry;

        shader.printBlockNames();
      }
    }

    void createDefaultBuffers(const Camera &camera) 
    {
      glm::mat4 globalMatrices[2];
      globalMatrices[0] = camera.perspective;
      globalMatrices[1] = camera.view;
      createBuffer("GlobalMatrices", globalMatrices, UB_GLOBALMATRICES);

      glm::mat4 matrices[65];
      createBuffer("Matrices", matrices, UB_MATRICES);
    }

    void updateViewMatrix(const Camera &camera)
    {
      updateBuffer("GlobalMatrices", camera.view, sizeof(glm::mat4));
    }

    void bindBuffers()
    {
      for (auto &elem : uniformBufferMap) {
        GLuniformBuffer &uniformBuffer = *elem.second;
        uniformBuffer.bind();
      }
    }

    template <typename T>
    GLuniformBuffer *createBuffer(const std::string blockName, const T &data, const int &index) 
    {
      std::unique_ptr<GLuniformBuffer> uniformBuffer(new GLuniformBuffer(blockName));

      uniformBuffer->create(data, index);
      uniformBufferMap[blockName] = std::move(uniformBuffer);

      return uniformBufferMap[blockName].get();
    }

    template <typename T>
      void updateBuffer(std::string blockName, const T &data) 
      {
        GLuniformBuffer &uniformBuffer = *uniformBufferMap.at(blockName);
        uniformBuffer.update(data, 0);
      }

    template <typename T>
      void updateBuffer(std::string blockName, const std::vector<T> &data) 
      {
        GLuniformBuffer &uniformBuffer = *uniformBufferMap.at(blockName);
        uniformBuffer.update(data, 0);
      }

    template <typename T>
      void updateBuffer(std::string blockName, const T &data, GLintptr offset) 
      {
        GLuniformBuffer &uniformBuffer = *uniformBufferMap.at(blockName);
        uniformBuffer.update(data, offset);
      }
};




#endif

