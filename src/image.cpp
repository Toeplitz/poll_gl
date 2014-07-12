#include "image.h"
#include "stb_image.h"


Image::Image()
{
}


Image::~Image()
{
  stbi_image_free(data);
}


bool Image::load(const std::string &filename)
{
  data = stbi_load(filename.c_str(), &width, &height, &components, 3);
  if (data == NULL) {
    std::cout << "Fragmic error: cannot load image '" << filename << "'" << std::endl;
    return false;
  }
  std::cout << filename << " loaded, size: " << width << " x " << height << ", " << components << std::endl;

  return true;
}
