#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include "texture.h"

struct Material_Properties
{
  glm::vec3 Ka;
  glm::vec3 Kd;
  glm::vec3 Ks;
  float shininess;
};

class Material 
{
  public:
    std::unique_ptr<Texture> diffuse;
    std::unique_ptr<Texture> normal;
    std::unique_ptr<Texture> specular;

    Material_Properties material_block;

    Material();
    ~Material();

    void print(const int indent_level);
};

