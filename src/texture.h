#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <memory>
#include <vector>

#include "gldefaults.h"
#include "image.h"

class Texture {
public:
  std::unique_ptr <Image> image;
  std::string filename;
  GLuint gl_texture;

  Texture();
  ~Texture();
  bool loadImage(const std::string & _filename);


};

#endif
