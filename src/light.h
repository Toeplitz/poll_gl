#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "mesh.h"

class Node;


enum Light_Type
{
  LIGHT_UNDEFINED = 0,
  LIGHT_DIRECTIONAL = 1,
  LIGHT_SPOT = 2,
  LIGHT_POINT = 3,
  LIGHT_SPHERE = 4
};


enum Light_Volume
{
  LIGHT_VOLUME_SPHERE = 0 << 1,
  LIGHT_VOLUME_BOX = 0 << 2
};


struct Light_Properties
{
  /*
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
  glm::vec4 direction;
  */
  glm::fvec4 position;
};


class Light
{
  private:
    Light_Properties properties;
    glm::vec3 bias;
    Node *follow;
    std::unique_ptr<Mesh> volume;

  public:
    Light();
    ~Light();

    void                    bias_set(const glm::vec3 &bias);
    void                    print(const int indent_level);
    const Light_Properties &properties_get() const;
    void                    properties_set(const glm::vec3 ambient, 
                                           const glm::vec3 diffuse, const glm::vec3 specular);
    void                    properties_direction_set(const glm::vec3 &direction);
    void                    properties_position_set(const glm::vec3 &position);
    void                    properties_type_set(const unsigned int type);
    void                    node_follow_set(Node *node);
    Node                   *node_follow_get();
    Mesh                   *volume_mesh_create(const unsigned int shape, const float size);
    Mesh                   *volume_mesh_get();
};
