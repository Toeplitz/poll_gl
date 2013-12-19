#ifndef BUFFERS_H
#define BUFFERS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <string>

#include "glbuffer.h"

class Buffers
{
  private:
    std::unique_ptr<GLuniformBuffer> armatureBuffer;
    std::unique_ptr<GLuniformBuffer> matricesBuffer;
    std::unique_ptr<GLuniformBuffer> globalMatricesBuffer;
    std::unique_ptr<GLuniformBuffer> materialBuffer;
    std::unique_ptr<GLuniformBuffer> debugBuffer;

  public:

    Buffers()
    {
    }


    ~Buffers()
    {
    }


    void init()
    {
      {
        std::string blockName("GlobalMatrices");
        std::unique_ptr<GLuniformBuffer> uniformBuffer(new GLuniformBuffer(blockName));
        glm::mat4 matrix[2];
        uniformBuffer->create(matrix, UB_GLOBALMATRICES);
        globalMatricesBuffer = std::move(uniformBuffer);
      }

      {
        std::string blockName("Matrices");
        std::unique_ptr<GLuniformBuffer> uniformBuffer(new GLuniformBuffer(blockName));
        glm::mat4 matrix;
        uniformBuffer->create(matrix, UB_MATRICES);
        matricesBuffer = std::move(uniformBuffer);
      }

      {
        std::string blockName("Armature");
        std::unique_ptr<GLuniformBuffer> uniformBuffer(new GLuniformBuffer(blockName));
        glm::mat4 matrices[64];
        uniformBuffer->create(matrices, UB_ARMATURE);
        armatureBuffer = std::move(uniformBuffer);
      }

      {
        std::string blockName("Material");
        std::unique_ptr<GLuniformBuffer> uniformBuffer(new GLuniformBuffer(blockName));
        glm::mat4 matrices[3];
        uniformBuffer->create(matrices, UB_MATERIAL);
        materialBuffer = std::move(uniformBuffer);
      }

      {
        glm::vec4 debug(0.5, 0.5, 0.5, 0);

        std::string blockName("Debug");
        std::unique_ptr<GLuniformBuffer> uniformBuffer(new GLuniformBuffer(blockName));
        uniformBuffer->create(debug, UB_DEBUG);
        debugBuffer = std::move(uniformBuffer);
      }
    }


    GLuniformBuffer *getGlobalMatricesBuffer()
    {
      return globalMatricesBuffer.get();
    }


    GLuniformBuffer *getMatricesBuffer()
    {
      return matricesBuffer.get();
    }


    GLuniformBuffer *getMaterialBuffer()
    {
      return materialBuffer.get();
    }


    GLuniformBuffer *getArmatureBuffer()
    {
      return armatureBuffer.get();
    }


    GLuniformBuffer *getDebugBuffer()
    {
      return debugBuffer.get();
    }

};


#endif
