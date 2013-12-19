#include "mesh.h"

Mesh::Mesh():
  vertexArray(), 
  model(glm::mat4(1.0))
{
}

Mesh::~Mesh(void)
{
}


  template <class T>
void Mesh::addVertexBuffer(const std::vector<T> &data, const int &attributeIndex)
{
  std::unique_ptr<GLvertexBuffer> vertexBufferPtr(new GLvertexBuffer());
  GLvertexBuffer &vertexBuffer= *vertexBufferPtr;
  vertexBuffer.create(data, attributeIndex);
  vertexBuffers.push_back(move(vertexBufferPtr));
}


  template <class T>
void Mesh::addIndexBuffer(const std::vector<T> &data)
{
  std::unique_ptr<GLindexBuffer> indexBufferPtr(new GLindexBuffer());
  GLindexBuffer &indexBuffer= *indexBufferPtr;
  indexBuffer.create(data);
  indexBuffers.push_back(move(indexBufferPtr));
}


const unsigned int Mesh::getSize()
{
  return indices.size();
}


Aabb Mesh::generateBoundingAabb() const
{
  Aabb aabb;

  if (vertices.empty()) {
    return aabb;
  }

  glm::vec3 minimum(std::numeric_limits<float>::max());
  glm::vec3 maximum(std::numeric_limits<float>::min());
  
  for (auto &v : vertices) {
    minimum = glm::min(minimum, v.position);
    maximum = glm::max(maximum, v.position);
  }

  std::cout << "minimum: " << minimum.x << ", " << minimum.y << ", " << minimum.z << std::endl;
  std::cout << "maximum: " << maximum.x << ", " << maximum.y << ", " << maximum.z << std::endl;

  aabb.setBounds(minimum.x, minimum.y, minimum.z,
                 maximum.x, maximum.y, maximum.z);

  return aabb;
}


void Mesh::create()
{
  std::vector<glm::vec4> tempVert;
  std::vector<glm::vec4> tempNormal;
  std::vector<glm::vec4> tempWeight;
  std::vector<glm::ivec4> tempBoneIndex;
  std::vector<glm::vec2> tempUv;
  std::vector<GLshort> tempIndices;
  size_t n = vertices.size();

  aabb = generateBoundingAabb();

  for (size_t i = 0; i < aabb.vertices.size(); i++) {
    std::cout << "AABB: (" << aabb.vertices[i].position.x << ", " << aabb.vertices[i].position.y << "," << aabb.vertices[i].position.z << ")" << std::endl;
    tempVert.push_back(glm::vec4(aabb.vertices[i].position, 1.0));
    tempNormal.push_back(glm::vec4(0, 0, 0, 1.0));
    tempWeight.push_back(glm::vec4(0, 0, 0, 0));
    tempBoneIndex.push_back(glm::ivec4(0,0,0,0));
    tempUv.push_back(glm::vec2(0,0));
  }
  std::cout << "tempVert size: " << tempVert.size() << std::endl;
  std::cout << "indicies size: " << indices.size() << std::endl;

  for (size_t i = 0; i < aabb.indices.size(); i++) {
    tempIndices.push_back(aabb.indices[i]);
  }


  for (size_t i = 0; i < n; i++) {
    tempVert.push_back(glm::vec4(vertices[i].position, 1.0));
    tempNormal.push_back(glm::vec4(vertices[i].normal, 1.0));
    tempWeight.push_back(vertices[i].weights);
    tempBoneIndex.push_back(vertices[i].bones);
    tempUv.push_back(vertices[i].uv);
  }

  for (size_t i = 0; i < indices.size(); i++) {
    tempIndices.push_back(indices[i]);
  }

  std::cout << "tempVert size: " << tempVert.size() << std::endl;
  std::cout << "indicies size: " << indices.size() << std::endl;

  addIndexBuffer(tempIndices);
  addVertexBuffer(tempVert, 0);
  addVertexBuffer(tempNormal, 1);
  addVertexBuffer(tempWeight, 2);
  addVertexBuffer(tempBoneIndex, 3);
  addVertexBuffer(tempUv, 4);
  vertexArray.create(indexBuffers, vertexBuffers);


}


void Mesh::update(GLuniformBuffer *buffer)
{
  buffer->update(model, 0);
}

void Mesh::updateAabb(GLuniformBuffer *buffer)
{
  buffer->update(model * aabb.transform, 0);
}



