#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include "texture.h"

class Material 
{
  public:
    std::unique_ptr<Texture> texture;

    Material();
    ~Material();
};

#endif
