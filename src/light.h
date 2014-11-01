#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using glm::vec4;
using glm::vec3;
using glm::mat4;

class Node;
class Mesh;


struct Light_Properties
{
  vec4 position = vec4(0, 0, 0, 1);
  vec4 color = vec4(0.5f);
  vec4 direction = vec4(0, -1, 0, 0);
  mat4 projection = mat4(1.f);
  mat4 view = mat4(1.f);
  int type = 0;
};


class Light
{
  private:
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
