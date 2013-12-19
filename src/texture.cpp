#include "texture.h"

using std::string;
using std::cout;
using std::endl;

Texture::Texture()
{

}

Texture::~Texture()
{

}

bool Texture::loadImage(const string & _filename)
{
  std::unique_ptr < Image > imagePtr(new Image());
  image = std::move(imagePtr);

  filename = _filename;

  if (!image->load(filename)) {
    return false;
  }

  return true;
}

void Texture::upload()
{
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  /*

     std::vector<GLubyte> temp(4*image->width*image->height);
     for(int j = 0;j<image->height;++j)
     for(int i = 0;i<image->width;++i)
     {
     size_t index = j*image->width + i;
     temp[4*index + 0] = 0xFF*(j/10%2)*(i/10%2); // R
     temp[4*index + 1] = 0xFF*(j/13%2)*(i/13%2); // G
     temp[4*index + 2] = 0xFF*(j/17%2)*(i/17%2); // B
     temp[4*index + 3] = 0xFF;                   // A
     }

     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->width, image->height, 
     0, GL_RGBA, GL_UNSIGNED_BYTE, &temp[0]);
   */

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height,
               0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

}
