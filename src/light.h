#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Node;
class Light;



struct Light_Properties
{
  glm::vec4 position_bias = glm::vec4(0.f);
  glm::vec4 color = glm::vec4(1.f);
  glm::vec4 direction = glm::vec4(0, -1, 0, 0);
  int type = 0;
};


class Light
{
  private:
    Light_Properties properties;
    Node *node_ptr = nullptr;

  public:
    enum Light_Type
    {
      UNDEFINED = 0,
      DIRECTIONAL = 1,
      SPOT = 2,
      POINT = 3
    };

    Node                   *node_ptr_get();
    void                    node_ptr_set(Node *node);
    void                    print(const int indent_level);
    void                    properties_bias_set(const glm::vec3 &bias);
    void                    properties_color_set(const glm::vec3 &color);
    void                    properties_direction_set(const glm::vec3 &direction);
    const Light_Properties &properties_get() const;
    void                    properties_type_set(const unsigned int type);
};
