#include "texture.h"

/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Texture::Texture()
{
}


Texture::~Texture()
{
  std::cout << "Deleting texture " << filename << std::endl;
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


bool Texture::image_load(const std::string &filename)
{
  std::unique_ptr <Image> image_ptr(new Image());
  image = std::move(image_ptr);
  this->filename = filename;

  if (!image->load(filename)) {
    std::cout << "Error: coult not load image" << std::endl;
    return false;
  }

  return true;
}

