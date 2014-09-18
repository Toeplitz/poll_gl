#include "light.h"
#include "mesh.h"
#include "utils.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


Gimbal_Nodes &Light::gimbal_nodes_get()
{
  return gimbal_nodes;
}


const unsigned int &Light::illumination_type_get()
{
  return illumination_type;
}


void Light::illumination_type_set(const unsigned int illumination_type)
{
  this->illumination_type = illumination_type;
}


Node *Light::node_ptr_get()
{
  return node_ptr;
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
  std::cout << "\tPosition: " <<  glm::to_string(properties.position) << std::endl;
  std::cout << "\tColor: " <<  glm::to_string(properties.color) << std::endl;
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


void Light::properties_position_set(const glm::vec3 &position)
{
  properties.position.x = position.x;
  properties.position.y = position.y;
  properties.position.z = position.z;
}


void Light::properties_type_set(const unsigned int type)
{
  properties.type = type;
}


unsigned int Light::properties_type_get()
{
  return properties.type;
}
