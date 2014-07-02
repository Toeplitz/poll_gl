#include "material.h"

Material::Material()
{
  diffuse.reset(nullptr);
  normal.reset(nullptr);
  specular.reset(nullptr);
}

Material::~Material()
{
}



