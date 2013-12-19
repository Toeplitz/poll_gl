#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "aabb.h"
#include "glbuffer.h"


class Mesh {

  private:



    template <class T>
      void addVertexBuffer(const std::vector<T> &data, const int &attributeIndex);
    template <class T>
      void addIndexBuffer(const std::vector<T> &data);

    Aabb generateBoundingAabb() const;

  public:
    Aabb aabb;
    std::vector<Vertex> vertices;
    std::vector<GLshort> indices;
    GLvertexArray vertexArray;
    std::vector<std::unique_ptr<GLvertexBuffer>> vertexBuffers;
    std::vector<std::unique_ptr<GLindexBuffer>> indexBuffers;
    glm::mat4 model;


    Mesh();
    ~Mesh();

    const unsigned int getSize();

    void update(GLuniformBuffer *buffer);
    void updateAabb(GLuniformBuffer *buffer);
    void create();
};

#endif
