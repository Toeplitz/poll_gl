#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


enum Light_Type
{
  LIGHT_UNDEFINED = 0,
  LIGHT_DIRECTIONAL = 1,
  LIGHT_SPOT = 2,
  LIGHT_POINT = 3,
  LIGHT_SPHERE = 4
};


struct Light_Properties
{
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
  glm::vec4 direction;
  glm::vec4 position;
  int type;
  glm::ivec3 buffer;
};


class Light
{
  private:
    Light_Properties properties;
    glm::vec3 bias;

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
};
