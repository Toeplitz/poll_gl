#include "texture.h"

/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Texture::Texture()
{
  std::unique_ptr<Image> image_ptr(new Image());
  image = std::move(image_ptr);
  gl_texture = 0;
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
  this->filename = filename;

  if (!image->load(filename)) {
    std::cout << "Error: could not load image: '" << filename << "'" << std::endl;
    return false;
  }

  return true;
}


Image &Texture::image_get()
{
  return *image.get();
}


