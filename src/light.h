#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Light_Properties
{
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
  glm::vec4 position;
};


class Light
{
  private:
    Light_Properties properties;

  public:
    Light();
    ~Light();

    const Light_Properties &properties_get() const;
    void                    properties_set(const glm::vec3 ambient, 
                                           const glm::vec3 diffuse, const glm::vec3 specular);
    void create();

};
