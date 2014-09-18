#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Node;
class Mesh;


struct Gimbal_Nodes 
{
      Node *center;
      Node *x;
      Node *y;
      Node *z;
};


struct Light_Properties
{
  glm::vec4 position = glm::vec4(0, 0, 0, 1);
  glm::vec4 color = glm::vec4(0.5f);
  glm::vec4 direction = glm::vec4(0, -1, 0, 0);
  int type = 0;
};


class Light
{
  private:
    Gimbal_Nodes gimbal_nodes;
    Light_Properties properties;
    Node *node_ptr = nullptr;
    unsigned int illumination_type;


  public:
    enum Light_Type
    {
      UNDEFINED = 0,
      DIRECTIONAL = 1,
      SPOT = 2,
      POINT = 3,
      VOLUME = 4,
      GLOBAL = 5
    };

    Gimbal_Nodes           &gimbal_nodes_get();
    const unsigned int     &illumination_type_get();
    void                    illumination_type_set(const unsigned int illum_type);
    Node                   *node_ptr_get();
    void                    node_ptr_set(Node *node);
    void                    print(const int indent_level);
    void                    properties_color_set(const glm::vec3 &color);
    void                    properties_direction_set(const glm::vec3 &direction);
    const Light_Properties &properties_get() const;
    void                    properties_position_set(const glm::vec3 &position);
    void                    properties_type_set(const unsigned int type);
    unsigned int            properties_type_get();
};
