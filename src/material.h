#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include "glbuffer.h"
#include "texture.h"

class Material 
{
  private:
    bool initialized;

  public:
    unsigned int id;
    std::unique_ptr<Texture> texture;

    Material();
    ~Material();

    void create();
    void update(GLuniformBuffer *buffer);

};

#endif
