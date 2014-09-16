#include "image.h"
#include <cstring>
#include "../external/stb_image.h"


bool Image::load(const std::string &filename)
{
  unsigned char *buf = stbi_load(filename.c_str(), &width, &height, &components, 3);
  size_t size = width * height * components;
  data.assign(buf, buf + size);
  stbi_image_free(buf);

  if (&data_get() == NULL) {
    std::cout << "Fragmic error: cannot load image '" << filename << "'" << std::endl;
    return false;
  }

 // std::cout << filename << " loaded, size: " << width << " x " << height << ", " << components << std::endl;

  return true;
}


void Image::data_copy(unsigned char *buf, const int width, const int height)
{
  if (data.size() > 0) {
    std::cout << "Error: image data already exists" << std::endl;
    return;
  }
  size_t size = width * height;
  data.assign(buf, buf + size);
  this->width = width;
  this->height = height;
}


const unsigned char &Image::data_get() const
{
  return *data.data();
}


