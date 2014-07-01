#pragma once

#include <memory>
#include "texture.h"

class Material 
{
  public:
    std::unique_ptr<Texture> diffuse;
    std::unique_ptr<Texture> normal;

    Material();
    ~Material();
};

