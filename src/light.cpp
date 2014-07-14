#include "light.h"

Light::Light()
{

}


Light::~Light()
{

}


void Light::create()
{

}


const Light_Properties &Light::properties_get() const
{
  return properties;
}


void Light::properties_set(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular) 
{
  properties.ambient = glm::vec4(ambient, 0);
  properties.diffuse = glm::vec4(diffuse, 0);
  properties.specular = glm::vec4(specular, 0);
}
