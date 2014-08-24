#include "light.h"
#include "utils.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "manipulator.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Light::Light()
{
  properties_transform_set(glm::mat4(1.f));
}


Light::~Light()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Light::bias_set(const glm::vec3 &bias)
{
  this->bias = bias;
}


void Light::print(const int indent_level)
{
  std::cout << std::endl;
  indent(std::cout, indent_level);
  std::cout << "Light details: " << std::endl;

  /*
  std::cout << "\tType: ";
  switch (properties.type) {
    case LIGHT_DIRECTIONAL:
      std::cout << "directional";
      break;
    case LIGHT_SPOT:
      std::cout << "spot";
      break;
    case LIGHT_POINT:
      std::cout << "point";
      break;
    case LIGHT_UNDEFINED:
      std::cout << "undefined";
      break;
  }
  std::cout << std::endl;
  std::cout << "\tDirection: " <<  glm::to_string(properties.direction) << std::endl;
  std::cout << "\tPosition: " <<  glm::to_string(properties.position) << std::endl;
  std::cout << "\tAmbient color: " <<  glm::to_string(properties.ambient) << std::endl;
  std::cout << "\tDiffuse color: " <<  glm::to_string(properties.diffuse) << std::endl;
  std::cout << "\tSpecular color: " <<  glm::to_string(properties.specular) << std::endl;

  */
}


const Light_Properties &Light::properties_get() const
{
  return properties;
}


void Light::properties_set(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular) 
{
  /*
  properties.ambient = glm::vec4(ambient, 0);
  properties.diffuse = glm::vec4(diffuse, 0);
  properties.specular = glm::vec4(specular, 0);
  */
}


void Light::properties_direction_set(const glm::vec3 &direction)
{
  //properties.direction = glm::vec4(direction, 0);
}


void Light::properties_position_set(const glm::vec3 &position)
{
}


const glm::vec4 &Light::properties_position_get() const
{
}


void Light::properties_transform_set(const glm::mat4 transform)
{
}


const glm::mat4 &Light::properties_transform_get()
{
}


void Light::properties_type_set(const unsigned int type)
{
  //properties.type = type;
}


Node *Light::node_ptr_get()
{
  return this->node_ptr;
}
 

void Light::node_ptr_set(Node *node)
{
  this->node_ptr = node;
}

