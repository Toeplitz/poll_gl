#ifndef GLRENDERER_H
#define GLRENDERER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <memory>
#include <vector>

#include "armature.h"
#include "buffers.h"
#include "material.h"
#include "mesh.h"
#include "gldefaults.h"
#include "glbuffer.h"

#include "printmatrix.h"

class GLrenderer {
  private:
    GLuniformBuffer *armatureBuffer;
    GLuniformBuffer *matricesBuffer;
    GLuniformBuffer *materialBuffer;

  public:
    GLrenderer()
    {
    } 


    ~GLrenderer() 
    {
    }


    void init(Buffers &buffers)
    {
      armatureBuffer = buffers.getArmatureBuffer();
      matricesBuffer = buffers.getMatricesBuffer();
      materialBuffer = buffers.getMaterialBuffer();
    }


    void upload(const Node &node)
    {
      Mesh *mesh = node.mesh;
      Material *material = node.material;

      if (mesh) {
        mesh->create();
      }

      if (material) {
        material->create();
      }

    }


    void draw(const Node &node) {
      Armature *armature = node.armature;
      Material *material = node.material;
      Mesh *mesh = node.mesh;

      if (!mesh) {
        return;
      }

      if (armature) {
        armature->update(armatureBuffer);
      }

      if (material) {
        material->update(materialBuffer);
      }

//      mesh->aabb.transform = mesh->aabb.transform * node.globalTransform;
      mesh->updateAabb(matricesBuffer);
      mesh->vertexArray.bind();

      glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
      glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(GLushort)));
      glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(GLushort)));
      glLineWidth(2);

      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(1, 0);
      mesh->update(matricesBuffer);
      GLsizei count = (GLsizei) mesh->getSize();
      GLvoid *indices = (GLvoid *) (mesh->aabb.getNumIndices() * sizeof(GLushort));
      GLint baseVertex = (GLint) mesh->aabb.getNumVertices();

      glDrawElementsBaseVertex(GL_TRIANGLES, count, 
                               GL_UNSIGNED_SHORT, 
                               indices, 
                               baseVertex);
      mesh->vertexArray.unbind();
    }


};


#endif
