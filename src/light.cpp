#include "light.h"
#include "utils.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


Node *Light::node_ptr_get()
{
  return this->node_ptr;
}
 

void Light::node_ptr_set(Node *node)
{
  this->node_ptr = node;
}


void Light::print(const int indent_level)
{
  std::cout << std::endl;
  indent(std::cout, indent_level);
  std::cout << "Light details: " << std::endl;

  std::cout << "\tType: ";
  switch (properties.type) {
    case DIRECTIONAL:
      std::cout << "directional";
      break;
    case SPOT:
      std::cout << "spot";
      break;
    case POINT:
      std::cout << "point";
      break;
    case UNDEFINED:
      std::cout << "undefined";
      break;
  }
  std::cout << std::endl;
  std::cout << "\tDirection: " <<  glm::to_string(properties.direction) << std::endl;
  std::cout << "\tColor: " <<  glm::to_string(properties.color) << std::endl;
}


void Light::properties_bias_set(const glm::vec3 &bias)
{
  properties.position_bias = glm::vec4(bias, 0);
}


void Light::properties_color_set(const glm::vec3 &color)
{
  properties.color = glm::vec4(color, 0);
}


const Light_Properties &Light::properties_get() const
{
  return properties;
}


void Light::properties_direction_set(const glm::vec3 &direction)
{
  properties.direction = glm::vec4(direction, 0);
}

void Light::properties_type_set(const unsigned int type)
{
  properties.type = type;
}
