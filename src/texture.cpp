#include "texture.h"


Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::loadImage(const std::string &filename)
{
  std::unique_ptr <Image> image_ptr(new Image());
  image = std::move(image_ptr);
  this->filename = filename;

  if (!image->load(filename)) {
    return false;
  }

  return true;
}

