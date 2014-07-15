#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


enum Light_Type
{
  LIGHT_UNDEFINED,
  LIGHT_DIRECTIONAL,
  LIGHT_SPOT,
  LIGHT_POINT
};


struct Light_Properties
{
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
  glm::vec4 direction;
  glm::vec4 position;
  unsigned int type;
};


class Light
{
  private:
    Light_Properties properties;

  public:
    Light();
    ~Light();

    void                    print(const int indent_level);
    const Light_Properties &properties_get() const;
    void                    properties_set(const glm::vec3 ambient, 
                                           const glm::vec3 diffuse, const glm::vec3 specular);
    void                    properties_direction_set(const glm::vec3 &direction);
    void                    properties_position_set(const glm::vec3 &position);
    void                    properties_type_set(const unsigned int type);
    void create();

};
