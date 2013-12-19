#include "material.h"

Material::Material():
initialized(false)
{
}

Material::~Material()
{
}


void Material::create()
{
  if (initialized) {
    return;
  }

  if (texture) {
    texture->upload();
  }

  initialized = true;
}


void Material::update(GLuniformBuffer *buffer)
{

  if (texture) {
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
  }

  // UPDATE material buffer here.

}


