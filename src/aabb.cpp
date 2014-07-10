#include "aabb.h"


Aabb::Aabb():
  transform(glm::mat4(1))
{
 addVertex(-0.5, -0.5, -0.5);
 addVertex(0.5, -0.5, -0.5);
 addVertex(0.5,  0.5, -0.5);
 addVertex(-0.5,  0.5, -0.5);
 addVertex(-0.5, -0.5,  0.5);
 addVertex(0.5, -0.5,  0.5);
 addVertex(0.5,  0.5,  0.5);
 addVertex(-0.5,  0.5,  0.5);
 addIndex(0);
 addIndex(1);
 addIndex(2);
 addIndex(3);
 addIndex(4);
 addIndex(5);
 addIndex(6);
 addIndex(7);
 addIndex(0);
 addIndex(4);
 addIndex(1);
 addIndex(5);
 addIndex(2);
 addIndex(6);
 addIndex(3);
 addIndex(7);
}


Aabb::~Aabb()
{

}

void Aabb::addVertex(const float x, const float y, const float z)
{
  Vertex v;

  v.position.x = x;
  v.position.y = y;
  v.position.z = z;

  vertices.push_back(v);
}


void Aabb::addIndex(GLshort index)
{
  indices.push_back(index);
}


void Aabb::setBounds(float minX, float minY, float minZ,
                     float maxX, float maxY, float maxZ)
{
  center = (glm::vec3(minX, minY, minZ) + glm::vec3(maxX, maxY, maxZ)) / 2.f;
  size = glm::vec3(maxX - minX, maxY - minY, maxZ - minZ);

  transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);

  //std::cout << "center: (" << center.x << ", " << center.y << "," << center.z << ")" << std::endl;
  //std::cout << "size: (" << size.x << ", " << size.y << "," << size.z << ")" << std::endl;
}

